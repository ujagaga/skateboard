#include "server.h"
#include "wifi_connection.h"
#include "bldcMotor.h"
#include "config.h"
#include "peripherals.h"


/* Initialization */
void setup(void) {
  delay(500);
  Serial.begin(115200); 
  Serial.println("\n\n");

  WIFIC_init_AP();
  SERVER_init(); 
  BLDCM_init();
  PERIPHERALS_init();
}

/* Main infinite while loop */
void loop(void) {     
  SERVER_process();  
  BLDCM_process();
  PERIPHERALS_process();
}
