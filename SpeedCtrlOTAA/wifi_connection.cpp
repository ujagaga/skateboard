#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <DNSServer.h>
#include "web_socket.h"
#include "config.h"
#include "SpeedCtrlOTAA.h"


char myApName[64] = {0};    /* Array to form AP name based on read MAC */
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;

/* Initiates a local AP */
void configAP(void){ 
  String wifi_statusMessage;

  Serial.println("\nStarting AP");  
  WiFi.mode(WIFI_AP);  
  WiFi.begin();
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  boolean result = WiFi.softAP(myApName);  
  if(result == true){
    wifi_statusMessage = "Running in AP mode. SSID: " + String(myApName) + ", IP: 192.168.1.1";  
  }else{
    wifi_statusMessage = "Failed to switch to AP mode.";
  }
  Serial.println(wifi_statusMessage);
  
  MAIN_setStatusMsg(wifi_statusMessage);
}

void WIFIC_process(void){ 
  dnsServer.processNextRequest();
}

void WIFIC_init(void){   
  String ApName = AP_NAME_PREFIX + WiFi.macAddress();
  ApName.toCharArray(myApName, ApName.length() + 1);
  
  configAP(); 

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);
}


