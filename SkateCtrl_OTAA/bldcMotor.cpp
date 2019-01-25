#include "SkateCtrl_OTAA.h"
#include "bldcMotor.h"

#define CMD_LEVEL_MAX             (100u)
#define ANALOG_LEVEL_MAX          (1023u)
#define MAX_TIMEOUT               (1000u)
#define MIN_RELATIVE_SPEED        (2u)
#define OLD                       (0u)
#define NEW                       (1u)
#define STOP                      (0u)

static uint8_t currentCmdVal = 0;
static unsigned long halSensTimestamp[2] = {0};
static uint16_t relativeSpeed = 0;
static uint16_t lastHalSensorTickTimespan = 0;
static unsigned long cmdTimestamp = 0;
static unsigned long executeTimestamp = 0;
static bool accelerateFlag = false;
static bool breakFlag = false;
static bool cruiseFlag = false;

bool cmdExpired(void)
{
  return (millis() - cmdTimestamp) > CMD_TIMEOUT;
}

bool executeRefreshTimeoutReached(void)
{
  return (millis() - executeTimestamp) > EXEC_REFRESH_TIMEOUT;
}

uint8_t BLDCM_getCurrentCmdVal( void ){
  return currentCmdVal;
}

uint16_t BLDCM_getRelativeSpeed( void ){
  return relativeSpeed;
}

bool BLDCM_setCommand(user_cmd_t cmd)
{
  bool ack = true;
  cmdTimestamp = millis();

  switch(cmd){
    case cmd_none:
      accelerateFlag = false;
      breakFlag = false;
      break;
      
    case cmd_accelerate:
      accelerateFlag = true;
      breakFlag = false;
      break;      
      
    case cmd_stop:
      accelerateFlag = false;
      breakFlag = true;
      break; 

    case cmd_cruiseCtrlOn:
      cruiseFlag = true;
      break;

    case cmd_cruiseCtrlOff:
      cruiseFlag = false;
      break;

    default:
      accelerateFlag = false;
      breakFlag = false;
      cruiseFlag = false;
      cmdTimestamp = 0;
      ack = false;
      break;
    
  }

  return ack;
}

/* Set command analog level in percentage */
void BLDCM_writeCmd(uint8_t val)
{ 
  uint32_t level = val;

  if(level > CMD_LEVEL_MAX){
    level = CMD_LEVEL_MAX;
  }
  
  if(level > 0){
    digitalWrite(PIN_EL, HIGH);
  }else{
    digitalWrite(PIN_EL, LOW);
  }
  
  currentCmdVal = level;

  level = level * 10 + level / 4;

  if(level > ANALOG_LEVEL_MAX){
    level = ANALOG_LEVEL_MAX;
  }    

  analogWrite(PIN_CMD, level);
}

void handleTahoCount() {
  halSensTimestamp[OLD] = halSensTimestamp[NEW];
  halSensTimestamp[NEW] = millis();
}

void BLDCM_init(void)
{
  pinMode(PIN_EL, OUTPUT);
  pinMode(PIN_CMD, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_EL, LOW);
  digitalWrite(PIN_LED, HIGH);
  BLDCM_writeCmd(0);

  pinMode(PIN_TAHO, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_TAHO), handleTahoCount, RISING);
}

void BLDCM_process(void)
{
  uint16_t halSensorTickTimespan = halSensTimestamp[NEW] - halSensTimestamp[OLD];

  if((millis() - halSensTimestamp[OLD]) < MAX_TIMEOUT)
  {
    
    relativeSpeed = MAX_TIMEOUT / halSensorTickTimespan;

    if(relativeSpeed < MIN_RELATIVE_SPEED){
      BLDCM_writeCmd(STOP);
    }else{
      /* Process user request */
      if(!cmdExpired()){
        if(executeRefreshTimeoutReached()){
          if(accelerateFlag){          
            uint8_t level = currentCmdVal * 1.1;
  
            if(currentCmdVal > CMD_LEVEL_MAX){
              if(level > CMD_LEVEL_MAX){
                level = CMD_LEVEL_MAX;
              }
              
              BLDCM_writeCmd(level);
            }
          }else if(breakFlag){
            
          }
        }
      }else{
        accelerateFlag = false;
      }
    }
    
  }else{
    BLDCM_writeCmd(STOP);
  }

  lastHalSensorTickTimespan = halSensorTickTimespan;
}

