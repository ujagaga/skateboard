#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include "config.h"


WebSocketsClient webSocket;
static volatile uint32_t beaconTimestamp = 0;


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:  
    {
      
    }break;
    case WStype_CONNECTED: 
    {
      Serial.println("Connected...");
    }break;
    case WStype_TEXT:  
    {     
      if(payload[0] == BEACON_MSG){
        beaconTimestamp = millis();
        //Serial.println("B");
      }
    }break;
    case WStype_BIN:     
      break;
  }
}


void WS_init(void){
  webSocket.begin(SERVER_IP, SERVER_PORT, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(700);
  beaconTimestamp = millis();
}

void WS_send(String msg){
  Serial.println(msg);
  webSocket.sendTXT(msg);
}

void WS_process(void){
  webSocket.loop();
}

bool WS_checkIfConnected(){
  
  if((millis() - beaconTimestamp) > CONNECTION_TIMEOUT){
    // No server available. Go to sleep.
    Serial.println("SLEEP");
    digitalWrite(PIN_LED, LOW); 
    digitalWrite(PIN_LED2, HIGH);     
    digitalWrite(PIN_ENABLE, LOW);
        
    ESP.deepSleep(0);    
  }

  return ((millis() - beaconTimestamp) <= BEACON_TIMEOUT);
}
