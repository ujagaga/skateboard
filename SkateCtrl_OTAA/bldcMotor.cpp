/*
* A user command is expected to be set at rate faster than every 350ms using BLDCM_setCommand(). 
* If it times out the motor controll is disabled.
*/

#include "config.h"
#include "bldcMotor.h"

#define CMD_LEVEL_MAX             (100u)
#define ANALOG_LEVEL_MAX          (1023u)
#define MAX_TIMEOUT               (1000u)
#define MIN_RELATIVE_SPEED        (2u)
#define OLD                       (0u)
#define NEW                       (1u)
#define STOP                      (-1)
#define CMD_NONE                  (0)
#define FORWARD                   LOW
#define REVERSE                   HIGH
#define ENABLE                    HIGH
#define DISABLE                   LOW


static uint8_t currentCmdVal = 0;
static unsigned long halSensTimestamp = 0;
static uint16_t relativeSpeed = 0;
static uint16_t relativeCruiseSpeed = 0;
static unsigned long cmdTimestamp = 0;
static unsigned long executeTimestamp = 0;
static user_cmd_t userRequest = cmd_none;
static uint16_t halSensorTickTimespan = 0;

bool cmdExpired(void)
{
  return (millis() - cmdTimestamp) > CMD_TIMEOUT;
}

bool executeRefreshTimeoutReached(void)
{
  return (millis() - executeTimestamp) > EXEC_REFRESH_TIMEOUT;
}

/* Set command analog level in percentage */
static void writeCmd(int8_t val)
{ 
  int32_t level = val;

  if(level > CMD_LEVEL_MAX){
    level = CMD_LEVEL_MAX;
  }

  if(level < 0){    
    currentCmdVal = 0;    
  }else{    
    currentCmdVal = level;
  }

  if(level < 0){
    /* Stop requested */
    currentCmdVal = 0;
    digitalWrite(PIN_DIR, REVERSE);
    
    /* TODO: think of a better strategy */    
    if((relativeSpeed * 2) > (MAX_TIMEOUT/EXEC_REFRESH_TIMEOUT)){
      /* Keep breaking */
      level = 10;      
    }else{      
      level = 0;
    }    
  }else{
    digitalWrite(PIN_DIR, FORWARD);
    
    currentCmdVal = level;    

    level = currentCmdVal * 10 + currentCmdVal / 4;

    if(level > ANALOG_LEVEL_MAX){
      level = ANALOG_LEVEL_MAX;
    }
  }
  
  analogWrite(PIN_CMD, level);

  executeTimestamp = millis();
}

void handleTahoCount() {
  halSensorTickTimespan = millis() - halSensTimestamp;
  halSensTimestamp = millis();
}

uint16_t BLDCM_getRelativeSpeed( void ){
  return relativeSpeed;
}

void BLDCM_setCommand(user_cmd_t cmd)
{ 
  if(cmd == cmd_cruise){
    relativeCruiseSpeed = relativeSpeed;
  }

  userRequest = cmd;
    
  cmdTimestamp = millis();  
}

void BLDCM_init(void)
{
  pinMode(PIN_EL, OUTPUT);
  pinMode(PIN_CMD, OUTPUT);
  pinMode(PIN_DIR, OUTPUT);
  digitalWrite(PIN_EL, ENABLE);
  digitalWrite(PIN_DIR, FORWARD);
  writeCmd(CMD_NONE);

  pinMode(PIN_TAHO, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_TAHO), handleTahoCount, RISING);
}

void BLDCM_process(void)
{ 

  if((millis() - halSensTimestamp) < MAX_TIMEOUT)
  {    
    relativeSpeed = MAX_TIMEOUT / halSensorTickTimespan;

    if(relativeSpeed < MIN_RELATIVE_SPEED){
      writeCmd(CMD_NONE);
    }else{
      /* Process user request, but only if we are mooving. */
      uint8_t level;
      
      if(cmdExpired()){
        userRequest = cmd_none;
      }
      
      switch(userRequest){
        case cmd_accelerate:
        {
          if(executeRefreshTimeoutReached()){ 
            if(currentCmdVal < 10){
              level = currentCmdVal + 1;
            }else{
              level = currentCmdVal * 1.1; 
            }
                        
            writeCmd(level);            
          }
        }break;

        case cmd_cruise:
        {
          if(executeRefreshTimeoutReached()){
            if(relativeCruiseSpeed < relativeSpeed){
              /* Accelerate */
              if(currentCmdVal < 10){
                level = currentCmdVal + 1;
              }else{
                level = currentCmdVal * 1.1; 
              }
            }else{
              /* slow down */
              if(currentCmdVal < 10){
                level = currentCmdVal - 1;
              }else{
                level = currentCmdVal * 0.7; 
              }

              if(level < 0){
                level = 0;
              }              
            }

            writeCmd(level);   
          }
        }break;

        case cmd_stop:
        {
          writeCmd(STOP);
        }break;

        default:
          writeCmd(CMD_NONE);
          break;
      }            
    }
    
  }else{
    writeCmd(CMD_NONE);
    relativeSpeed = 0;
  }
}
