#include <WiFiUdp.h>
#include "SkateCtrl_OTAA.h"

#define MAX_UDP_RX_SIZE   (255u)

static WiFiUDP Udp;
static char msgBuffer[MAX_UDP_RX_SIZE];        /* buffer for incoming packets */              
static IPAddress IP_Remote(255,255,255,255);   


void UDP_init(){
  Udp.begin(UDP_PORT); 
}

void UDP_send(String msg){
  if(msg.length() < MAX_UDP_RX_SIZE){  
    msg.toCharArray(msgBuffer, msg.length() + 1);
    msgBuffer[msg.length()] = 0; 
    Udp.beginPacket(IP_Remote, UDP_PORT);
    Udp.write(msgBuffer);
    Udp.endPacket();   
  }else{
    Serial.print("UDP message too long");   
  }
}

void UDP_process(){
  int packetSize = Udp.parsePacket();
  
  if(packetSize > 1)
  {
    IP_Remote = Udp.remoteIP();
    
    // receive incoming UDP packets
    int len = Udp.read(msgBuffer, MAX_UDP_RX_SIZE);
    msgBuffer[len] = 0;
    String rxMsg = String(msgBuffer);

    /* Parse rxMsg and then respond using UDP_send(response) */
     
  }
}
