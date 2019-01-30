#include <ESP8266WiFi.h>
#include "config.h"


static char myApName[] = AP_NAME; 
static const char myApPass[] = AP_PASS;

void WIFIC_init(void){   

  WiFi.setAutoReconnect(true);
  WiFi.begin(AP_NAME, AP_PASS);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(AP_NAME);    

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
    ESP.wdtFeed();
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());  
}
