#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include "config.h"

WebSocketsClient webSocket;
static bool connectionStatus = false;
static volatile uint32_t noConnectionTimestamp = 0;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:  
    {
      connectionStatus = false; 
      if(noConnectionTimestamp == 0){
        noConnectionTimestamp = millis();
      }
    }break;
    case WStype_CONNECTED: 
    {
      connectionStatus = true;
      noConnectionTimestamp = 0;
    }break;
    case WStype_TEXT:      
      break;
    case WStype_BIN:     
      break;
  }
}


void WS_init(void){
  webSocket.begin(SERVER_IP, SERVER_PORT, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(1000);
  noConnectionTimestamp = 0;
}

void WS_send(String msg){
  webSocket.sendTXT(msg);
}

void WS_process(void){
  webSocket.loop();
}

bool WS_checkIfConnected(){
  
  if((noConnectionTimestamp > 0) && ((millis() - noConnectionTimestamp) > CONNECTION_TIMEOUT)){
    // No server available. Go to sleep.
    Serial.println("SLEEP");
    digitalWrite(PIN_LED, LOW); 
    digitalWrite(PIN_ENABLE, LOW);
        
    ESP.deepSleep(0);    
  }

  return connectionStatus;
}
