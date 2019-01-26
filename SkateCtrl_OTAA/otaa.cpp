#include <ArduinoOTA.h>
#include "config.h"

void OTAA_init() {
  ArduinoOTA.setHostname(AP_NAME);
  ArduinoOTA.setPassword(AP_PASS);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else 
      type = "filesystem";
  });
  
  ArduinoOTA.onEnd([]() {});  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {});  
  ArduinoOTA.onError([](ota_error_t error) {});
  ArduinoOTA.begin();
}

void OTAA_process(void){
  ArduinoOTA.handle();
}

