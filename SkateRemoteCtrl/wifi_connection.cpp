#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "config.h"

static volatile uint32_t noConnectionTimestamp = 0;
static char devName[32] = {0};    /* Array to form device name based on MAC */

bool WIFIC_checkIfConnected(){  
  bool connrctionStatus = false;
  
  if((noConnectionTimestamp > 0) && ((millis() - noConnectionTimestamp) > CONNECTION_TIMEOUT)){
    // No server available. Go to sleep.

    Serial.println("SLEEP");
    digitalWrite(PIN_LED, LOW); 
    digitalWrite(PIN_ENABLE, LOW);
        
    ESP.deepSleep(0);    
  }

  if(WiFi.status() == WL_CONNECTED) { 
    noConnectionTimestamp = 0;
    connrctionStatus = true;
  }else if(noConnectionTimestamp == 0){
    noConnectionTimestamp = millis();
  }

  return connrctionStatus;
}

void WIFIC_init(void){ 
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  String deviceName = DEV_NAME_PREFIX + mac;
  deviceName.toCharArray(devName, deviceName.length() + 1);  

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);

  noConnectionTimestamp = 0;
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
