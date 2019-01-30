#include "udp.h"
#include "wifi_connection.h"
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
static bool cruiseFlagPending = false;
static unsigned long cmdTimestamp = 0;
static user_cmd_t lastCmd = cmd_none;
static uint8_t cmdNoneCount = 0;

static void setCruiseCtrl(bool activate){

  cruiseFlag = activate;
  
  if(activate){
    digitalWrite(PIN_LED, LOW);
  }else{
    digitalWrite(PIN_LED, HIGH);
  }
}

static void setCmd(user_cmd_t cmd){
  if(cmd != lastCmd){
    /* Command changed. Force execution. */ 
    cmdTimestamp = 0;
  }
  
  if((millis() - cmdTimestamp) > CMD_TIMEOUT){
    /* Time to reppeat command to refresh it. */
    if(cmd == cmd_none){
      /* No button is pressed */
      if(cruiseFlagPending){
        /* Cruise flag toggle requested. */
        cruiseFlagPending = false;
        cruiseFlag = !cruiseFlag;
      }
      
      if(cruiseFlag){
        UDP_send(CMD_CRUISE);
      }else{
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
      cruiseFlag = false;
    }else if(cmd == cmd_accelerate){
      UDP_send(CMD_ACCEL);
      cruiseFlag = false;
    }else if(cmd == cmd_cruise){
      /* Cruise button pressed. Prepare to toggle the cruise flag. */
      cruiseFlagPending = true;      
    }

    cmdTimestamp = millis();
  }  

  lastCmd = cmd;
  if(cmd != cmd_none){
    cmdNoneCount = 0;
  }
}

/* Initialization */
void setup(void) {
  Serial.begin(115200,SERIAL_8N1,SERIAL_TX_ONLY); /* Use only tx, so rx can be used as GPIO */   
    
  WIFIC_init(); 
  UDP_init(); 
  OTAA_init();  

  /* Init buttons and the LED */
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);
  pinMode(PIN_ACCEL, INPUT_PULLUP);
  pinMode(PIN_CRUISE, INPUT_PULLUP);
  pinMode(PIN_BREAK, INPUT_PULLUP);
}

/* Main infinite while loop */
void loop(void) {  
  ESP.wdtFeed();  
  
  if(millis() < OTAA_UPDATE_TIMEOUT)
  { /* After a timeout stop processing OTAA to save processor time. */
    OTAA_process(); 
  }

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
