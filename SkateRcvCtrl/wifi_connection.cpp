#include <ESP8266WiFi.h>
#include "config.h"


static char myApName[] = AP_NAME; 
static const char myApPass[] = AP_PASS;
IPAddress apIP(192, 168, 10, 1);

void WIFIC_init_AP(void){   
    String wifi_statusMessage;
    
    Serial.println("\nStarting AP");  
    WiFi.mode(WIFI_AP);  
    WiFi.begin();
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));    
    boolean result = WiFi.softAP(myApName, myApPass);  
    if(result){
      wifi_statusMessage = "Running in AP mode. SSID: " + String(myApName) + ", IP: " + WiFi.softAPIP().toString();  
    }else{
      wifi_statusMessage = "Failed to switch to AP mode.";
    }
    Serial.println(wifi_statusMessage); 
}
