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
 

uint8_t BLDCM_getCurrentCmdVal( void ){
  return currentCmdVal;
}

uint16_t BLDCM_getRelativeSpeed( void ){
  return relativeSpeed;
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
      
    }
    
  }else{
    BLDCM_writeCmd(STOP);
  }

  lastHalSensorTickTimespan = halSensorTickTimespan;
}

