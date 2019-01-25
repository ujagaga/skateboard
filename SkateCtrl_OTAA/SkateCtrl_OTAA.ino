#include "udp.h"
#include "wifi_connection.h"
#include "SkateCtrl_OTAA.h"
#include "otaa.h"
#include "bldcMotor.h"


void setup(void) {
  Serial.begin(115200,SERIAL_8N1,SERIAL_TX_ONLY); /* Use only tx, so rx can be used as GPIO */   
  //ESP.eraseConfig();
    
  WIFIC_init(); 
  ESP.wdtFeed();
  UDP_init(); 
  ESP.wdtFeed();
  OTAA_init();
  BLDCM_init();
}


void loop(void) {  
  ESP.wdtFeed();
  UDP_process();
  ESP.wdtFeed();
  OTAA_process(); 
  ESP.wdtFeed();
  BLDCM_process();
}
