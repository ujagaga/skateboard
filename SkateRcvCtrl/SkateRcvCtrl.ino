#include "server.h"
#include "wifi_connection.h"
#include "ota.h"
#include "bldcMotor.h"
#include "config.h"
#include "ota.h"


/* Initialization */
void setup(void) {
  delay(500);
  Serial.begin(115200); 
  Serial.println("\n\n");

  WIFIC_init_AP();
  SERVER_init(); 
  BLDCM_init();
}

/* Main infinite while loop */
void loop(void) {     
  if(OTA_updateInProgress()){ 
    OTA_process(); 
  }else{
    SERVER_process();  
    BLDCM_process();
  }
}
