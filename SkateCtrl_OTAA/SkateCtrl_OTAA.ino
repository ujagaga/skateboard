#include "udp.h"
#include "wifi_connection.h"
#include "config.h"
#include "otaa.h"
#include "bldcMotor.h"

/* Initialization */
void setup(void) {
  Serial.begin(115200,SERIAL_8N1,SERIAL_TX_ONLY); /* Use only tx, so rx can be used as GPIO */   
    
  WIFIC_init(); 
  UDP_init(); 
  OTAA_init();
  BLDCM_init();
}

/* Main infinite while loop */
void loop(void) {  
  ESP.wdtFeed();
  UDP_process();  
  BLDCM_process();

  if(millis() < OTAA_UPDATE_TIMEOUT)
  { /* After a timeout stop processing OTAA to save processor time. */
    OTAA_process(); 
  }
}
