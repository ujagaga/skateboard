#include "config.h"
#include "bldcMotor.h"
#include "server.h"
#include "current.h"

#define MAX_HAL_TIMEOUT                 (60 * 1000)/(MINIMUM_SPEED_RPM * HAL_TICKS_PER_REV)
#define ONE_KMPH_TIMEOUT                (25000ul)
#define ANALOG_LEVEL_MAX                (1023)
#define ANALOG_LEVEL_MIN                (160u)
#define MIN_RELATIVE_SPEED              (2u)
#define FORWARD                         LOW
#define REVERSE                         HIGH
#define ENABLE                          LOW
#define DISABLE                         HIGH
#define SPEED_NONE_CMD                  (0)
#define CMD_SOUND_FREQ                  (2500u)
#define CMD_OFF_SOUND_FREQ              (1500u)
#define HONK_SOUND_FREQ                 (600u)
#define SL_ON_SOUND_FREQ                (2500u)
#define SL_OFF_SOUND_FREQ               (1500u)
#define SL_TGL_SOUND_FREQ               (2000u)
#define SPEED_TO_VOLTAGE_MULTIPLIER     (28u)


static volatile uint32_t halSensTimestamp = 0;
static volatile uint32_t avgHalSensorTickTimespan = MAX_HAL_TIMEOUT * 2;
static volatile uint32_t relativeSpeed = 0;        // relativeSpeed/10 => speed in kmph
static unsigned long executeTimestamp = 0;
static user_cmd_t activeCmd = cmd_none;
static uint16_t currentCmdVal = 0;
static bool stop_request_flag = false;
static uint32_t target_speed = 0;

static void setBrake(uint32_t level){
  BLDCM_setSpeed(0);
  if(level > 1){
    digitalWrite(PIN_BRAKE, HIGH); 
  }else{
    digitalWrite(PIN_BRAKE, LOW); 
  }
}

static bool executeRefreshTimeoutReached(void)
{
  return (millis() - executeTimestamp) > EXEC_REFRESH_TIMEOUT;
}

static bool mooving(void){ 
 //return true;
#ifdef TEST_MODE
  return true;
#else
  if((micros() - halSensTimestamp) < 100){
      return ((relativeSpeed/10) > 0);
  }else{
      return false;  
  }
#endif
}

void handleTahoCount() {   
  uint32_t lastHalSensorTickTimespan = micros() - halSensTimestamp;
  halSensTimestamp = micros(); 
  avgHalSensorTickTimespan = (avgHalSensorTickTimespan + lastHalSensorTickTimespan)/2; 
  relativeSpeed = (ONE_KMPH_TIMEOUT * 10) / avgHalSensorTickTimespan; 
}

uint16_t BLCMD_getCmdVoltage(){
  return (uint16_t)currentCmdVal;
}

void BLDCM_setTargetSpeed(uint8_t percent){
  if(percent > 100){
    percent = 100;
  }

  target_speed = (SPEED_LIMIT * percent)/10;  

  /* Adjust current control voltage to value near current speed, so we do not wait long to accelerate. */
  uint32_t targetCtrlVoltage = (relativeSpeed / 10) * SPEED_TO_VOLTAGE_MULTIPLIER;
  if((targetCtrlVoltage + ACCELERATE_INCREMENT) < currentCmdVal){
    BLDCM_setSpeed(targetCtrlVoltage);
  }

}


/* Set command analog level in range 0..1023 */
void BLDCM_setSpeed(uint32_t val)
{ 
  uint32_t level = val;
  if(level > ANALOG_LEVEL_MAX){
    level = ANALOG_LEVEL_MAX;
  }
  
//  if(!mooving()){     
//    level = 0;
//  }
  
  currentCmdVal = level;

  /* We have an inverting switch HW, so invert the value. */
  level = ANALOG_LEVEL_MAX - level;
  digitalWrite(PIN_DIR, FORWARD);  
  analogWrite(PIN_CMD, level);  
  
}


void BLDCM_init(void)
{
  pinMode(PIN_EL, OUTPUT);
  pinMode(PIN_CMD, OUTPUT);
  pinMode(PIN_DIR, OUTPUT);
  pinMode(PIN_SPEAKER, OUTPUT);
  pinMode(PIN_BRAKE, OUTPUT);

  digitalWrite(PIN_BRAKE, LOW); 
  digitalWrite(PIN_EL, ENABLE);
  digitalWrite(PIN_DIR, FORWARD);  
  digitalWrite(PIN_SPEAKER, LOW); 
  
  BLDCM_setSpeed(SPEED_NONE_CMD);

  pinMode(PIN_TAHO, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_TAHO), handleTahoCount, RISING);
   
}

uint16_t BLCMD_getSpeed(void){ 
  return (relativeSpeed/10);
}

void BLDCM_process(void)
{ 
  user_cmd_t userRequest = SERVER_getLastReceivedCommand();
  
  if(userRequest != activeCmd){
    /* Force new command processing. */
    executeTimestamp = 0;    
  } 
  
  if(executeRefreshTimeoutReached()){  
    user_cmd_t lastCmd = activeCmd;
    activeCmd = userRequest;
    
    uint32_t level = 0;
    
    switch(userRequest){
      case cmd_accelerate:
      {       
        if(stop_request_flag){
          stop_request_flag = false;
          setBrake(0);    
          level = ANALOG_LEVEL_MIN;
        }else if(currentCmdVal < ANALOG_LEVEL_MIN){
          level = ANALOG_LEVEL_MIN;
        }else{
          if(relativeSpeed < target_speed){    
            if(((relativeSpeed * 100) / target_speed) > 80){
              /* Getting close to target. Slow down acceleration pace. */
               level = currentCmdVal + (ACCELERATE_INCREMENT / 2);        
            }else{
              level = currentCmdVal + ACCELERATE_INCREMENT;
            }
          }else{           
            level = currentCmdVal - ACCELERATE_INCREMENT;
          }
        }        
      }break;  

      case cmd_none:
      {
        stop_request_flag = false; 
        setBrake(0);    
      }break;
           
      case cmd_stop:
      {
        stop_request_flag = true;  
        BLDCM_setSpeed(0);
        delay(100);
        setBrake(300);
      }break;

      default:          
        break;
    }     

    BLDCM_setSpeed(level); 
    executeTimestamp = millis();   
  }
}
