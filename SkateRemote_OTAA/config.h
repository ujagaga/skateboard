#ifndef SKATECTRL_OTAA_H
#define SKATECTRL_OTAA_H

#include <ESP8266WiFi.h>

#define AP_NAME                 "ujagaga_skate"
#define AP_PASS                 "SkatePass123"
#define UDP_PORT                (4213u)  
#define CMD_TIMEOUT             (150u)
#define OTAA_UPDATE_TIMEOUT     (10000u)

#define PIN_ACCEL               D1 
#define PIN_CRUISE              D2
#define PIN_BREAK               D3
#define PIN_LED                 D4

/* Commands */
#define CMD_NONE      "CMD_0"
#define CMD_ACCEL     "CMD_1"
#define CMD_CRUISE    "CMD_2"
#define CMD_STOP      "CMD_3"

#endif
