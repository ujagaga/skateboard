#include "udp.h"
#include "wifi_connection.h"
#include "config.h"
#include "otaa.h"

#define LED_LEVEL_MAX             (100u)

static const int LEDPIN = D4;        

static int currentLedVal = 0;            /* Current LED level. used in forming HTML and id */


int MAIN_getCurrentLedVal( void ){
  return currentLedVal;
}

/* Set LED level in percentage */
void MAIN_writeLED(int val)
{ 
  double level = val;
  
  if(level < 0){
      level = 0;
  }
  if(level > LED_LEVEL_MAX){
    level = LED_LEVEL_MAX;
  }
  currentLedVal = level;

  level = level * 10 + level / 4;

  if(level > 1023){
    level = 1023;
  }    

  analogWrite(LEDPIN, level);
}




void setup(void) {
  Serial.begin(115200,SERIAL_8N1,SERIAL_TX_ONLY); /* Use only tx, so rx can be used as GPIO */   
  //ESP.eraseConfig();
  pinMode(LEDPIN, OUTPUT);
  MAIN_writeLED(0);
  
  WIFIC_init(); 
  ESP.wdtFeed();
  UDP_init(); 
  ESP.wdtFeed();
  OTAA_init();
}


void loop(void) {  
  ESP.wdtFeed();
  UDP_process();
  ESP.wdtFeed();
  WIFIC_process();   
  ESP.wdtFeed();
  OTAA_process(); 
}
