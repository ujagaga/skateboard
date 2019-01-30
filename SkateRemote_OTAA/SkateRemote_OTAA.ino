#include "udp.h"
#include "config.h"
#include "otaa.h"

#define CMD_NONE_MAX_COUNT    (2)

typedef enum
{
  cmd_none = 0,
  cmd_accelerate,
  cmd_cruise,
  cmd_stop  
}user_cmd_t;
 
static bool cruiseFlag = false;               
static bool cruiseFlagPending = false;        /* Helps to set the cruise controll after the butten is released. */
static unsigned long cmdTimestamp = 0;        /* Stores the timestamp of the last sent command so we can calculate the timeout to re-send the active command. */
static user_cmd_t lastCmd = cmd_none;         /* Stores the las requested command so we can detect when a different command has been requested. */
static uint8_t cmdNoneCount = 0;              /* Stores the number of times the cmd_none has been sent. */
static bool batteryChargingStarted = false;   /* Used to charge the battery only once. */

/* Sets cruise controll flag and the D1 MINI onboard LED */
static void setCruiseCtrl(bool activate){
  cruiseFlag = activate;
  
  if(activate){
    digitalWrite(PIN_LED, LOW);
  }else{
    digitalWrite(PIN_LED, HIGH);
  }
}

/* Calculates if it is time to re-send the active command. */
static bool cmdTimedOut(void){
  return ((millis() - cmdTimestamp) > CMD_TIMEOUT);
}

/* process the requested command. */
static void setCmd(user_cmd_t cmd){
  if(cmd != lastCmd){
    /* Command changed. Force execution. */ 
    cmdTimestamp = 0;
    lastCmd = cmd;  
  }

  if(cmd != cmd_none){
    cmdNoneCount = 0;
  }
  
  if(cmdTimedOut()){
    /* Time to reppeat command to refresh it. */
    if(cmd == cmd_none){
      /* No button is pressed */
      
      if(cruiseFlagPending){
        /* Cruise flag toggle requested and cruise button has just been released. */
        cruiseFlagPending = false;
        
        setCruiseCtrl(!cruiseFlag);        
      }
      
      if(cruiseFlag){
        /* No button is pressed, but cruise controll has been requested. */
        UDP_send(CMD_CRUISE);
      }else{
        /* No cruise controll has been requested, so we should send the CMD_NONE a few times. */
        if(cmdNoneCount < CMD_NONE_MAX_COUNT){
          UDP_send(CMD_NONE);
          cmdNoneCount++;
        }else{
          /* We do not need to send CMD_NONE any longer. It has been received by now and even if it has not, 
          the last command has expired and CMD_NONE was set as default. */
        }
      }
    }else if(cmd == cmd_stop){
      UDP_send(CMD_STOP);
      setCruiseCtrl(false);
    }else if(cmd == cmd_accelerate){
      UDP_send(CMD_ACCEL);
      setCruiseCtrl(false);
    }else if(cmd == cmd_cruise){
      /* Cruise button pressed. Prepare to toggle the cruise flag. */
      cruiseFlagPending = true;      
    }

    cmdTimestamp = millis();
  }    
}

/* Used to make sure that the battery charging was initiated only once */
static void processBatteryCharging(void){
  int chargeMeasure = analogRead(BAT_PIN); 

  if(chargeMeasure < BATT_FULL_COUNT){  
    if((!batteryChargingStarted) && (chargeMeasure > BATT_MIN_VOLTAGE) && (chargeMeasure < BATT_LOW_COUNT) && (digitalRead(CHARGE_V_DETECT_PIN) == HIGH)){
      /* Battery has not been charged since power on,
      *  minimum battery voltage is detected, so battery is connected,
      *  battery voltage is low enough to need re-charging and
      *  5V source which can charge the battery is connected */
      digitalWrite(CHARGE_PIN, HIGH);
      batteryChargingStarted = true;
    }
  }else{
    digitalWrite(CHARGE_PIN, LOW);
  }  
}

/* Initialization */
void setup(void) {
  Serial.begin(115200,SERIAL_8N1,SERIAL_TX_ONLY); /* Use only tx, so rx can be used as GPIO */   
    
  WiFi.setAutoReconnect(true);
  WiFi.begin(AP_NAME, AP_PASS); 
   
  UDP_init(); 
  OTAA_init();  

  /* Init buttons and the LED */
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);
  pinMode(CHARGE_PIN, OUTPUT);
  digitalWrite(CHARGE_PIN, LOW);
  pinMode(PIN_ACCEL, INPUT_PULLUP);
  pinMode(PIN_CRUISE, INPUT_PULLUP);
  pinMode(PIN_BREAK, INPUT_PULLUP);
  pinMode(CHARGE_V_DETECT_PIN, INPUT);
}

/* Main infinite while loop */
void loop(void) {
  
  if(millis() < OTAA_UPDATE_TIMEOUT)
  { /* After a timeout stop processing OTAA to save processor time and avoid poor performance. */
    OTAA_process(); 
  }

  processBatteryCharging();

  /* Process buttons */
  if( digitalRead(PIN_BREAK)){
    setCmd(cmd_stop);
  }else if( digitalRead(PIN_ACCEL)){
    setCmd(cmd_accelerate);
  }else if( digitalRead(PIN_CRUISE)){
    setCmd(cmd_cruise);
  }else{
    setCmd(cmd_none);
  }
}
