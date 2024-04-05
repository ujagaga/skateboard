#include "config.h"
#include "bldcMotor.h"
#include "server.h"

#define MIN_SPEED                         (60u)

// Input voltage is 2,5V - 5V
#define MIN_BRAKE_LEVEL                 (700u)   
#define MAX_BRAKE_LEVEL                 (1023u)   


static uint32_t executeTimestamp = 0;
static user_request_t activeCmd;
static uint16_t currentSpeed = 0;
static bool stop_request_flag = false;
static uint32_t target_speed = 0;
static uint8_t brakeIntensity = 0;


static void processStopRequest(){
  if(brakeIntensity < MIN_BRAKE_LEVEL){
    brakeIntensity = MIN_BRAKE_LEVEL;   
  }

  if(brakeIntensity < MAX_BRAKE_LEVEL){
    brakeIntensity += 10;
  }
  analogWrite(PIN_CMD, 0);  
  analogWrite(PIN_BRAKE, brakeIntensity); 
}


static void setCurrentSpeed()
{ 
  if(currentSpeed > SPEED_LIMIT){
    currentSpeed = SPEED_LIMIT;
  }
  
  if(currentSpeed > 0){
    brakeIntensity = 0;
    analogWrite(PIN_BRAKE, brakeIntensity);    // Disable brake;  
  }
  analogWrite(PIN_CMD, currentSpeed);   

 // Serial.println(currentSpeed);
}

static void setTargetSpeed(uint8_t percent)
{
  if(percent > 100){
    percent = 100;
  }
  target_speed = (SPEED_LIMIT * percent)/10;
}

static bool executeRefreshTimeoutReached(void)
{
  return (millis() - executeTimestamp) > EXEC_REFRESH_TIMEOUT;
}

void BLDCM_init(void)
{
  pinMode(PIN_BRAKE, OUTPUT);
  digitalWrite(PIN_BRAKE, LOW); 
   
  pinMode(PIN_CMD, OUTPUT);   
  analogWrite(PIN_CMD, 0);  

  activeCmd.cmd = cmd_none;
  activeCmd.intensity = 0;
   
}

void BLDCM_disable(void){
  analogWrite(PIN_CMD, 0);  
  analogWrite(PIN_BRAKE, 0);
  
  target_speed = 0;
  stop_request_flag = false;
}

uint16_t BLCMD_getSpeed(void)
{ 
  return currentSpeed;
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

    switch(userRequest.cmd)
    {
      case cmd_accelerate:
      { 
        //Serial.print("A");     
        
        if(stop_request_flag){
          stop_request_flag = false;    
          brakeIntensity = 0;
          analogWrite(PIN_BRAKE, brakeIntensity);   
                   
          currentSpeed = MIN_SPEED;
        }else if(currentSpeed < MIN_SPEED){
          currentSpeed = MIN_SPEED;
        }else{
         // Serial.print("3 ");
          setTargetSpeed(activeCmd.intensity);
          
          if(currentSpeed < target_speed)
          {    
            if(((currentSpeed * 100) / target_speed) > 80)
            {  /* Getting close to target. Slow down acceleration pace. */
               currentSpeed = currentSpeed + (ACCELERATE_INCREMENT / 2);    
            }else
            {
              currentSpeed = currentSpeed + ACCELERATE_INCREMENT;
            }
          }else
          {      
            currentSpeed = currentSpeed - ACCELERATE_INCREMENT;
          }
        }
      }break;  

      case cmd_none:
      {
        stop_request_flag = false; 
        brakeIntensity = 0;
        currentSpeed = 0;
        target_speed = 0;
        analogWrite(PIN_BRAKE, brakeIntensity); 
      }break;
           
      case cmd_stop:
      {
        //Serial.println("B");     
        stop_request_flag = true;       
        currentSpeed = 0;
        setCurrentSpeed();
        processStopRequest();
        
      }break;

      default:          
        break;
    }     
   
    setCurrentSpeed();     
    
    executeTimestamp = millis(); 
  } 
}
