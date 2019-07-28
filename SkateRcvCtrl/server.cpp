#include <WebSocketsServer.h>
#include "config.h"
#include "bldcMotor.h"
#include "SkateRcvCtrl.h"
#include "ota.h"
#include "wifi_connection.h"
#include "current.h"

static WebSocketsServer wsServer = WebSocketsServer(TCP_PORT);
static user_cmd_t lastReceivedCommand = cmd_none;


static void serverEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t len)
{ 
  switch (type) {
    case WStype_DISCONNECTED:             // the websocket is disconnected
      lastReceivedCommand = cmd_none;
      break;
    case WStype_CONNECTED:                // a new websocket connection is established     
      break;
    case WStype_TEXT:                     // new text data is received
    {   
      if (payload[0] == 'a') {           
        lastReceivedCommand = cmd_accelerate;  
        //BLDCM_setTargetSpeed(100);   
        if(len > 2){
          uint8_t newSpeed = (uint8_t)atoi((char*)&payload[1]);
          BLDCM_setTargetSpeed(newSpeed);
        }else{
          BLDCM_setTargetSpeed(100);        
        }
      }else if(payload[0] == 's'){
        lastReceivedCommand = cmd_stop;
      }else if(payload[0] == 'm'){
        String currVal = String("S:" + String(BLCMD_getSpeed()) + " C:" + String(CURR_getVal()) +  " V:" + String(BLCMD_getCmdVoltage()));
        wsServer.broadcastTXT(currVal);         
      }else if(payload[0] == 'u'){
        /* Switch to OTAA */
        lastReceivedCommand = cmd_none;
        BLDCM_setSpeed(cmd_none);                
        OTA_init(); 
      }else{
        lastReceivedCommand = cmd_none;
      }      
    }break;
  }
}

user_cmd_t SERVER_getLastReceivedCommand(void){
  return lastReceivedCommand;
}

void SERVER_init(){
  wsServer.begin(); 
  wsServer.onEvent(serverEvent);  
}

void SERVER_process() 
{
  wsServer.loop(); 
}
