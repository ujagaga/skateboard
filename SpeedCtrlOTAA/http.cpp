#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <pgmspace.h>
#include "logo.h"
#include "http.h"
#include "udp.h"
#include "web_socket.h"
#include "config.h"
#include "SpeedCtrlOTAA.h"

static const char HTML_BEGIN[] PROGMEM = R"(
<!DOCTYPE HTML>
<html>
  <head>
    <meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
    <title>Ujagaga WiFi LED Control</title>
    <style>
      body { background-color: white; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }
      .contain{width: 100%;}.center_div{margin:0 auto; max-width: 400px;position:relative;}
    </style>
  </head>
  <body>
)";

static const char HTML_END[] PROGMEM = "</body></html>";

static const char INDEX_HTML_0[] PROGMEM = R"(
<style>
  .btn_b{border:0;border-radius:0.3rem;color:#fff;line-height:2.4rem;font-size:1.2rem;margin:2%;height:2.4rem;background-color:#1fa3ec;}
  .btn1{width:25%;}.btn2{width:85%;}
  .btn_cfg{border:0;border-radius:0.3rem;color:#fff;line-height:1.4rem;font-size:0.8rem;margin:1ch;height:2rem;width:15ch;background-color:#ff3300;}
  h1{text-align: center;font-size:1.5rem;}
  #m{-webkit-transform:rotate(-90deg);-moz-transform:rotate(-90deg);-o-transform:rotate(-90deg);transform:rotate(-90deg);
  position:absolute;right:-7.5rem;bottom:8rem; width:16rem;height:5%;}
</style>
<div class="contain">
  <div class="center_div">
)";

const char INDEX_HTML_1[] PROGMEM = R"(
    <h1>Ujagaga WiFi LED Control</h1>      
    <button class="btn_b btn2" type="button" onclick="setLevel(0);">OFF</button><br>
    <button class="btn_b btn1" type="button" onclick="setLevel(10);">1</button>
    <button class="btn_b btn1" type="button" onclick="setLevel(20);">2</button>
    <button class="btn_b btn1" type="button" onclick="setLevel(30);">3</button><br>
    <button class="btn_b btn1" type="button" onclick="setLevel(40);">4</button>
    <button class="btn_b btn1" type="button" onclick="setLevel(50);">5</button>
    <button class="btn_b btn1" type="button" onclick="setLevel(60);">6</button><br>
    <button class="btn_b btn1" type="button" onclick="setLevel(70);">7</button>
    <button class="btn_b btn1" type="button" onclick="setLevel(80);">8</button>
    <button class="btn_b btn1" type="button" onclick="setLevel(90);">9</button><br>
    <button class="btn_b btn2" type="button" onclick="setLevel(100);">ON</button>
    <meter id="m" value="" min="0" max="100"></meter> 
  </div>
  <hr>
  <p id='status'></p>  
  <br><button class="btn_cfg" type="button" onclick="location.href='/selectap';">Configure wifi</button><br/>
</div>
<script>
  var cn=new WebSocket('ws://'+location.hostname+':81/');
  cn.onopen=function(){
    cn.send('{"CURRENT":,"STATUS":}');    
  };
  cn.onmessage=function(e){
    var data=JSON.parse(e.data);
    if(data.hasOwnProperty('CURRENT')){       
      document.getElementById('m').value=parseInt(data.CURRENT);       
    } 
    if(data.hasOwnProperty('STATUS')){       
      document.getElementById('status').innerHTML=data.STATUS;       
    }
  };
  function setLevel(lev){  
    cn.send('{"CURRENT":' + lev + '}');
  }
</script>
)";



static ESP8266WebServer webServer(80);

static void showNotFound(void){
  webServer.send(404, "text/html; charset=iso-8859-1","<html><head> <title>404 Not Found</title></head><body><h1>Not Found</h1></body></html>");
}

String HTTP_getFeatures( void ){
  Serial.println("returnFeatures");
  String response = "{\"MAC\":\"";
  response += WiFi.macAddress();
  response += ":";
  response += DEV_ID;
  response += "\",\"MODEL\":\"ujagaga WiFi LED\"";
  response += ",\"CURRENT\":";
  response += String(MAIN_getCurrentLedVal());  
  response += "}";  
  return response;     
}

void showID( void ) {    
  Serial.println("show ID");
  webServer.send(200, "text/plain", HTTP_getFeatures());  
}

void showStartPage() {    
  Serial.println("showStartPage");
  String response = FPSTR(HTML_BEGIN);
  response += FPSTR(INDEX_HTML_0);
  response += FPSTR(LOGO);
  response += FPSTR(INDEX_HTML_1); 
  response += FPSTR(HTML_END);
  webServer.send(200, "text/html", response);  
}


void HTTP_process(void){
  webServer.handleClient(); 
}

void HTTP_init(void){
 
  webServer.on("/", showStartPage);
  webServer.on("/id", showID);
  webServer.on("/favicon.ico", showNotFound);
  webServer.onNotFound(showStartPage);
  
  webServer.begin();
}



