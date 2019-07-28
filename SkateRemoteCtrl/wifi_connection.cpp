#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "config.h"


static char devName[32] = {0};    /* Array to form device name based on MAC */

void WIFIC_init(void){ 
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  String deviceName = DEV_NAME_PREFIX + mac;
  deviceName.toCharArray(devName, deviceName.length() + 1);  

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
}

void WIFIC_connectToScate( void ){
  /* Configure WiFi connection. ESP8266 will automatically connect to the AP when it is available. */
  WiFi.begin(AP_NAME, AP_PASS); 
}

void WIFIC_connectToMainNetwork( void ){  
  WiFi.begin(AP_MAIN_NAME, AP_MAIN_PASS); 
}

char* WIFIC_getDeviceName( void ){
  return devName;
}
