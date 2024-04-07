#include "config.h"
#include "bldcMotor.h"
#include "server.h"

#define MIN_SPEED                    (60u)
#define BRAKE_LEVEL                 (1023)   


static uint32_t executeTimestamp = 0;
static user_request_t activeCmd;
static uint16_t currentSpeed = 0;
static bool stop_request_flag = false;
static uint32_t target_speed = 0;


static void processStopRequest(){
#ifdef REVERSE_SPEED_POLARITY
  analogWrite(PIN_CMD, 1024);
#else
  analogWrite(PIN_CMD, 0);
#endif  
  analogWrite(PIN_BRAKE, BRAKE_LEVEL); 
}


static void setCurrentSpeed()
{ 
  if(currentSpeed > SPEED_LIMIT){
    currentSpeed = SPEED_LIMIT;
  }
  
  if(currentSpeed > 0){
    analogWrite(PIN_BRAKE, 0);    // Disable brake;  
  }
#ifdef REVERSE_SPEED_POLARITY
  analogWrite(PIN_CMD, 1024 - currentSpeed);
#else
  analogWrite(PIN_CMD, currentSpeed);  
#endif 
   

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
#ifdef REVERSE_SPEED_POLARITY
  analogWrite(PIN_CMD, 1024);
#else
  analogWrite(PIN_CMD, 0);
#endif 

  activeCmd.cmd = cmd_none;
  activeCmd.intensity = 0;
   
}

void BLDCM_disable(void){
#ifdef REVERSE_SPEED_POLARITY
  analogWrite(PIN_CMD, 1024);
#else
  analogWrite(PIN_CMD, 0);
#endif   
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
          analogWrite(PIN_BRAKE, 0);   
                   
          currentSpeed = MIN_SPEED;
        }else if(currentSpeed < MIN_SPEED){
          currentSpeed = MIN_SPEED;
        }else{
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
        currentSpeed = 0;
        target_speed = 0;
        analogWrite(PIN_BRAKE, 0); 
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
