#include <WiFiUdp.h>
#include "config.h"
  
#define MAX_UDP_RX_SIZE         (10u)

static WiFiUDP Udp;  

void UDP_init(){
  Udp.begin(UDP_PORT); 
}

void UDP_send(String msg){
  int msgLen = msg.length();  
  
  char txBuffer[MAX_UDP_RX_SIZE];
    
  msg.toCharArray(txBuffer, msgLen + 1);
  txBuffer[msgLen] = 0; 
  
  Udp.beginPacket(WiFi.gatewayIP(), UDP_PORT);
  Udp.write(txBuffer);
  Udp.endPacket();     
}
