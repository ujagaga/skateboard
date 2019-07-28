#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include "config.h"

WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:      
      break;
    case WStype_CONNECTED: 
      break;
    case WStype_TEXT:      
      break;
    case WStype_BIN:     
      break;
  }
}


void WS_init(void){
  webSocket.begin(SERVER_IP, SERVER_PORT, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

void WS_send(String msg){
  webSocket.sendTXT(msg);
}

void WS_process(void){
  webSocket.loop();
}
