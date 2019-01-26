#include <WiFiUdp.h>
#include "config.h"
#include "bldcMotor.h"
  
#define MAX_UDP_RX_SIZE         (255u)
#define CMD_PREFIX              "CMD_"  /* To be used for commands like CMD_0, CMD_1,... */ 
#define CMD_NUMBER_SEPARATOR    '-'
#define STAT_REQUEST            "STAT"
#define CMD_INVALID             (-1)
#define CMD_OK                  "OK"
#define CMD_ERR                 "ERR"

static WiFiUDP Udp;                 
static IPAddress IP_Remote(255,255,255,255);   

/* We could use available c functions for converting string to integer, 
 * but we already know that we will only have one digit, so this is faster. */
static int8_t charToUint(char c){
   uint8_t result = CMD_INVALID;
   
   if ((c >= '0') || (c <= '9')){
      result = c - '0';
   }

   return result;
}

void UDP_init(){
  Udp.begin(UDP_PORT); 
}

void UDP_send(String msg){
  int msgLen = msg.length();
  
  if(msgLen < MAX_UDP_RX_SIZE){
    char txBuffer[msgLen + 1];
      
    msg.toCharArray(txBuffer, msgLen);
    txBuffer[msgLen] = 0; 
    
    Udp.beginPacket(IP_Remote, UDP_PORT);
    Udp.write(txBuffer);
    Udp.endPacket();   
  }else{
    Serial.print("UDP message too long");   
  }
}

/* Parse received UDP packages containing commands or status request. */
void UDP_process(){
  int packetSize = Udp.parsePacket();
  
  if((packetSize > 1) && (packetSize < MAX_UDP_RX_SIZE)){   
    IP_Remote = Udp.remoteIP();
    
    char rxBuffer[packetSize + 1]; 
    int len = Udp.read(rxBuffer, packetSize);
    rxBuffer[len] = 0;
    String rxMsg = String(rxBuffer);

    /* Parse rxMsg and then respond using UDP_send(response) */
    if(rxMsg.startsWith(CMD_PREFIX)){
      char cmdNumber = rxMsg.indexOf(CMD_NUMBER_SEPARATOR);
      user_cmd_t cmd = (user_cmd_t)charToUint(cmdNumber);

      if(cmd == CMD_INVALID){
        UDP_send(CMD_ERR);
      }else{
        UDP_send(CMD_OK);

        BLDCM_setCommand(cmd);
      }    
      
    }else if(rxMsg.startsWith(STAT_REQUEST)){
      uint16_t relativeSpeed = BLDCM_getRelativeSpeed();
      String response = String(relativeSpeed);
      UDP_send(response);
    }
  }
}
