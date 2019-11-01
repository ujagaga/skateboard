#include "config.h"
#include "bldcMotor.h"
#include "server.h"


#define MAX_HAL_TIMEOUT                 (60 * 1000)/(MINIMUM_SPEED_RPM * HAL_TICKS_PER_REV)
#define ONE_KMPH_TIMEOUT                (25000ul)
#define ANALOG_LEVEL_MAX                (1023)
#define ANALOG_LEVEL_MIN                (160u)
#define MIN_RELATIVE_SPEED              (2u)
#define SPEED_TO_VOLTAGE_MULTIPLIER     (28u)
#define LOW_BRAKE                       (255u)
#define HONK_DURATION                   (600u)


static volatile uint32_t halSensTimestamp = 0;
static volatile uint32_t relativeSpeed = 0;        // relativeSpeed/10 => speed in kmph
static uint32_t executeTimestamp = 0;
static user_request_t activeCmd;
static uint32_t currentCmdVal = 0;
static bool stop_request_flag = false;
static uint32_t target_speed = 0;
static uint32_t tahoCount = 0;
static uint32_t lastTahoCount = 0;
static bool moovingFlag = false;
static uint8_t requestedBrakeIntensity = 255;
static uint8_t brakeCounter = 0;
static volatile uint32_t honkTimestamp = 0;


static void processHonkRequest(){ 

  if(honkTimestamp > 0){
    /* TODO: Activate honk pin here */

    
    if((millis() - honkTimestamp) > HONK_DURATION){
      honkTimestamp = 0;
    }
  }
}


static void processStopRequest(){
  if(brakeCounter == requestedBrakeIntensity){
    brakeCounter = 0;
    
    digitalWrite(PIN_BRAKE, HIGH); 
  }else{
    digitalWrite(PIN_BRAKE, LOW);   
  }

  brakeCounter++;  
}

/* Set command analog level in range 0..1023 */
static void setCurrentSpeed(uint32_t val)
{ 
  uint32_t level = val;
  if(level > ANALOG_LEVEL_MAX)
  {
    level = ANALOG_LEVEL_MAX;
  }
  
  if(!moovingFlag){     
    //level = 0;
  }
  
  currentCmdVal = level;

  if(val > 0){
    digitalWrite(PIN_BRAKE, LOW);   // Disable brake;  
  }
  analogWrite(PIN_CMD, level);   

//  Serial.print("M:");
//  Serial.println(level);
}

static void setTargetSpeed(uint8_t percent)
{
  if(percent > 100){
    percent = 100;
  }

  target_speed = (SPEED_LIMIT * percent)/10;  

  /* Adjust current control voltage to value near current speed, so we do not wait long to accelerate. */
  uint32_t targetCtrlVoltage = (relativeSpeed / 10) * SPEED_TO_VOLTAGE_MULTIPLIER;
  if((targetCtrlVoltage + ACCELERATE_INCREMENT) < currentCmdVal)
  {    
    setCurrentSpeed(targetCtrlVoltage);    
  }
}

static bool executeRefreshTimeoutReached(void)
{
  return (millis() - executeTimestamp) > EXEC_REFRESH_TIMEOUT;
}


void handleTahoCount() 
{   
  uint32_t lastHalSensorTickTimespan = micros() - halSensTimestamp;
  halSensTimestamp = micros(); 
  relativeSpeed = (ONE_KMPH_TIMEOUT * 10) / lastHalSensorTickTimespan; 
  
  if(stop_request_flag){
    processStopRequest();
  }
}

uint32_t BLCMD_getCmdVoltage()
{
  return currentCmdVal;
}

void BLDCM_init(void)
{

  pinMode(PIN_BRAKE, OUTPUT);
  digitalWrite(PIN_BRAKE, LOW); 
  
  pinMode(PIN_EL, OUTPUT);
  digitalWrite(PIN_EL, HIGH);
  
  pinMode(PIN_CMD, OUTPUT);   
  setCurrentSpeed(0);

  pinMode(PIN_TAHO, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_TAHO), handleTahoCount, RISING);

  activeCmd.cmd = cmd_none;
  activeCmd.intensity = 0;
   
}

void BLDCM_disable(void){
  analogWrite(PIN_CMD, 0);  
  digitalWrite(PIN_BRAKE, LOW);
  
  target_speed = 0;
  stop_request_flag = false;   

  pinMode(PIN_BRAKE, INPUT);
  pinMode(PIN_CMD, INPUT);
  
}

uint16_t BLCMD_getSpeed(void)
{ 
  return (relativeSpeed/10);
}

void BLDCM_process(void)
{ 
  user_request_t userRequest = SERVER_getLastReceivedCommand();
  
  if((userRequest.cmd != activeCmd.cmd) || (userRequest.intensity != activeCmd.intensity)){
    /* Force new command processing. */
    executeTimestamp = 0;  
  } 
  
  if(executeRefreshTimeoutReached())
  {  
    activeCmd.cmd = userRequest.cmd;
    activeCmd.intensity = userRequest.intensity;
    
    uint32_t level = 0;
    
    switch(userRequest.cmd)
    {
      case cmd_accelerate:
      {       
        if(stop_request_flag){
          stop_request_flag = false;    
          digitalWrite(PIN_BRAKE, LOW);   // Disable brake;            
          level = ANALOG_LEVEL_MIN;
        }else if(currentCmdVal < ANALOG_LEVEL_MIN){
          level = ANALOG_LEVEL_MIN;
        }else{

          setTargetSpeed(activeCmd.intensity);
          
          if(relativeSpeed < target_speed)
          {    
            if(((relativeSpeed * 100) / target_speed) > 80)
            {  /* Getting close to target. Slow down acceleration pace. */
               level = currentCmdVal + (ACCELERATE_INCREMENT / 2);        
            }else
            {
              level = currentCmdVal + ACCELERATE_INCREMENT;
            }
          }else
          {           
            level = currentCmdVal - ACCELERATE_INCREMENT;
          }
        }        
      }break;  

      case cmd_none:
      {
        stop_request_flag = false; 
        digitalWrite(PIN_BRAKE, LOW);   // Disable brake;  
      }break;
           
      case cmd_stop:
      {
        if(!stop_request_flag){
          stop_request_flag = true;  
          requestedBrakeIntensity = LOW_BRAKE;
          brakeCounter = 0;
        }else{
          if(activeCmd.intensity < 20){
            requestedBrakeIntensity = 5;      // Only every 5th motor hal pulse
          }else if(activeCmd.intensity < 40){
            requestedBrakeIntensity = 4;
          }else if(activeCmd.intensity < 65){
            requestedBrakeIntensity = 3;
          }else if(activeCmd.intensity < 90){
            requestedBrakeIntensity = 2;
          }else{
            requestedBrakeIntensity = 1;    // Highest brake intensity
          } 

//          Serial.print("B:");
//          Serial.println(requestedBrakeIntensity);
        }
      }break;

      case cmd_honk:
      {
        honkTimestamp = millis();
      }break;

      default:          
        break;
    }     

    setCurrentSpeed(level); 

    uint32_t tahoImpulses = tahoCount - lastTahoCount;
    lastTahoCount = tahoCount;
    moovingFlag = (tahoImpulses > 4);    
    
    executeTimestamp = millis(); 
  } 
}
