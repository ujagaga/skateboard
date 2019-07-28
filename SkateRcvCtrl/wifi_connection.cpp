#include <ESP8266WiFi.h>
#include "config.h"


static char myApName[] = AP_NAME; 
static const char myApPass[] = AP_PASS;
IPAddress apIP(192, 168, 10, 1);

void WIFIC_init_AP(void){   
    String wifi_statusMessage;

    //WiFi.disconnect(true);
    //ESP.reset();
    //delay(1000);
    
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

void WIFIC_init_STA(void){
  const char *ssid = "rada_i_slavica";
  const char *password = "ohana130315"; 

  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password); //Connect to wifi
 
  // Wait for connection  
  Serial.println("Connecting to Wifi");
  while (WiFi.status() != WL_CONNECTED) {   
    delay(500);
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  
}
