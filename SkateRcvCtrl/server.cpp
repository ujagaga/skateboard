#include "config.h"
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include "bldcMotor.h"
#include "ota.h"
#include "wifi_connection.h"
#include "server.h"
#include "peripherals.h"

#define CC_TIMEOUT        (1200u)
#define BEACON_TIMEOUT    (300u)

/* Generate the index page as a raw string */
static const char indexPage[] PROGMEM = R"=====(
<html>
<head><title>Skate Remote</title><link rel="shortcut icon" href="">
<style>
.ctrl{margin:0;width:80vw;height:25vh;border:2px solid #111;border-radius:2em;margin:2em auto;}
.ctrl h1{text-align center;vertical-align:middle;}
.ctrl:hover{background-color:#CCC;}
#acc{background:green;}
#stop{background:red;}
#honk{background:blue;}
#statMsg{width:100%;margin:1em auto;font-size:3em;}
</style></head>
<body>
<div class="ctrl" id="acc" ><h1>Accelerate</h1></div>
<div class="ctrl" id="stop" ><h1>Stop</h1></div> 
<div class="ctrl" id="honk" ><h1>Honk</h1></div>
<p id="statMsg" >Not connected.</p>
<script> 
var url='ws://192.168.10.1:4213';
var itemAcc=document.querySelector("#acc");
var itemStop=document.querySelector("#stop");
var itemHonk=document.querySelector("#honk");
var statusMsg=document.querySelector("#statMsg");
var curr=document.querySelector("#curr");
var cn=new WebSocket(url);
function connect(){cn=new WebSocket(url);}

cn.onopen=function(event){statusMsg.innerHTML="Connected.";};
cn.onclose=function(event){statusMsg.innerHTML="No connection!";};
cn.onmessage=function(event){if(event.data != 'B'){statMsg.innerHTML=event.data;}};

function send(c){  
console.log("Send:" + c);
try{cn.send(c);}catch(error){
console.error(error);
setTimeout(connect, 1000);
}}

itemAcc.addEventListener("mousedown",function(){send('a');},false);
itemAcc.addEventListener("mouseup",function(){send('n');},false);
itemAcc.addEventListener("mouseleave",function(){send('n');},false);
itemAcc.addEventListener("touchstart",function(){send('a');},false);
itemAcc.addEventListener("touchend",function(){send('n');},false);
itemStop.addEventListener("mousedown",function(){send('s');},false);
itemStop.addEventListener("mouseup",function(){send('n');},false);
itemStop.addEventListener("mouseleave",function(){send('n');},false);
itemStop.addEventListener("touchstart",function(){send('s');},false);
itemStop.addEventListener("touchend",function(){send('n');},false);
itemHonk.addEventListener("mousedown",function(){send('h');},false);
itemHonk.addEventListener("mouseup",function(){send('n');},false);
itemHonk.addEventListener("mouseleave",function(){send('n');},false);
itemHonk.addEventListener("touchstart",function(){send('h');},false);
itemHonk.addEventListener("touchend",function(){send('n');},false);

var stats=setInterval(statusTimer,500);
function statusTimer(){send('m');}
</script></body></html>
)=====";
/* ==============End of index page===============*/

static ESP8266WebServer httpServer(80);
static WebSocketsServer wsServer = WebSocketsServer(TCP_PORT);
static user_cmd_t lastReceivedCommand = cmd_none;
static uint8_t intensity = 0;
static volatile uint32_t clientCheckTimestamp = 0;
static volatile uint32_t beakonTimestamp = 0;

static void chkClientStatus() {
  if((millis() - clientCheckTimestamp) > CC_TIMEOUT){
    lastReceivedCommand = cmd_none;
    Serial.println("n");
  }
}

/* Sends a beacon at regular intervals as lifesign.
 *  Default WiFi implementation works but the station only detects connection loss after 5 seconds or more.
 *  We need this to be faster.
 */
static void processBeacon(){
  if((millis() - beakonTimestamp) > BEACON_TIMEOUT){
     wsServer.broadcastTXT("B");
     beakonTimestamp = millis();
  }
}

static void serverEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t len)
{   
  switch (type) {
    case WStype_DISCONNECTED:             // the websocket is disconnected
    {      
      if(num == 0){
        lastReceivedCommand = cmd_none;        
      }
    }break;
    case WStype_CONNECTED:                // a new websocket connection is established    
    { 

    }break;
    case WStype_TEXT:                     // new text data is received
    { 
      if(num != 0){ 
        //Serial.println("Not the first client connected! Refusing command.");
        return;
      }

      clientCheckTimestamp = millis();
      
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
        String currVal = String(BLCMD_getSpeed());

        wsServer.broadcastTXT(currVal);
      }else if(payload[0] == 'n'){
        lastReceivedCommand = cmd_none;        
      }else if(payload[0] == 'h'){
        PERIPHERALS_honk();
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

void homePage()
{
  httpServer.send(200, "text/html", indexPage);
}

void SERVER_init(){
  wsServer.begin(); 
  wsServer.onEvent(serverEvent);  
  
  httpServer.on("/", homePage);  
  httpServer.onNotFound(homePage);  
  httpServer.begin();
}

void SERVER_process() 
{
  wsServer.loop(); 
  chkClientStatus();
  processBeacon();
  httpServer.handleClient(); 
}
