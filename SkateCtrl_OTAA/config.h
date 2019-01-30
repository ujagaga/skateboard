#ifndef SKATECTRL_OTAA_H
#define SKATECTRL_OTAA_H

#include <ESP8266WiFi.h>

#define AP_NAME                 "ujagaga_skate"
#define AP_PASS                 "SkatePass123"
#define UDP_PORT                (4213u)  
#define CMD_TIMEOUT             (350u)
#define EXEC_REFRESH_TIMEOUT    (100u)
#define OTAA_UPDATE_TIMEOUT     (10000u)

#define PIN_EL                  D1 
#define PIN_TAHO                D2
#define PIN_CMD                 D3
#define PIN_DIR                 D4

#endif
