#include <ArduinoOTA.h>
#include "wifi_connection.h"

void OTAA_init() {
  ArduinoOTA.setHostname(myApName);
  ArduinoOTA.setPassword((const char *)"ohana13");

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

