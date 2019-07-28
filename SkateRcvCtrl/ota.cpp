#include <ArduinoOTA.h>
#include "config.h"

static bool updateStartedFlag = false;

bool OTA_updateInProgress(){
  return updateStartedFlag;
}

void OTA_init() {      
    /* Hostname defaults to esp8266-[ChipID] */
  ArduinoOTA.setHostname((const char *)AP_NAME);
    /* No authentication by default and can cause connectivity issues, so best do without it. */
  //ArduinoOTA.setPassword((const char *)AP_PASS);

  ArduinoOTA.onStart([]() {
    Serial.println("Start OTA update");
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
    ESP.restart();
  });  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  updateStartedFlag = true;

  Serial.println("OTA update mode started.");
}

void OTA_process(void){
  ArduinoOTA.handle();  
}
