#include <WebSocketsServer.h>
#include "config.h"
#include "bldcMotor.h"
#include "ota.h"
#include "wifi_connection.h"
#include "server.h"


static WebSocketsServer wsServer = WebSocketsServer(TCP_PORT);
static user_cmd_t lastReceivedCommand = cmd_none;
static uint8_t intensity = 0;


static void serverEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t len)
{ 
  switch (type) {
    case WStype_DISCONNECTED:             // the websocket is disconnected
      lastReceivedCommand = cmd_none;
      intensity = 0;
      break;
    case WStype_CONNECTED:                // a new websocket connection is established     
      break;
    case WStype_TEXT:                     // new text data is received
    {   
      if(len > 2){
        intensity = (uint8_t)atoi((char*)&payload[1]);        
      }else{
        intensity = 100;        
      }
      
      if (payload[0] == 'a') {           
        lastReceivedCommand = cmd_accelerate;  
      }else if(payload[0] == 's'){
        lastReceivedCommand = cmd_stop;
      }else if(payload[0] == 'm'){
        String currVal = String("S:" + String(BLCMD_getSpeed()) + " V:" + String(BLCMD_getCmdVoltage()));
        wsServer.broadcastTXT(currVal);         
      }else if(payload[0] == 'n'){
        lastReceivedCommand = cmd_none;
      }else if(payload[0] == 'u'){
        /* Switch to OTAA */
        lastReceivedCommand = cmd_none;
        BLDCM_disable();
        OTA_init(); 
      }else{
        lastReceivedCommand = cmd_none;
      }      
    }break;
  }
}

user_request_t SERVER_getLastReceivedCommand(void){
  user_request_t retVal;

  retVal.cmd = lastReceivedCommand;
  retVal.intensity = intensity;
  
  return retVal;
}

void SERVER_init(){
  wsServer.begin(); 
  wsServer.onEvent(serverEvent);  
}

void SERVER_process() 
{
  wsServer.loop(); 
}
