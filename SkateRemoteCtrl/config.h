#ifndef SKATECTRL_OTAA_H
#define SKATECTRL_OTAA_H

#include <ESP8266WiFi.h>

#define AP_NAME                 "ujagaga_skate"
#define AP_PASS                 "SkatePass123"
#define AP_MAIN_NAME            "rada_i_slavica"
#define AP_MAIN_PASS            "ohana130315"
#define DEV_NAME_PREFIX         "ScateRemote"   
#define SERVER_IP               "192.168.10.1"
#define SERVER_PORT             (4213u)  
#define CMD_TIMEOUT             (150u)
#define CONNECTION_TIMEOUT      (10000u)


/* ESP-01 pin defines */
#define PIN_ANALOG              (A0)
#define PIN_UARTTX              (1u)
#define PIN_UARTRX              (3u)
#define PIN_GPIO14              (14u)
#define PIN_GPIO04              (4u)

#define PIN_LED                 PIN_GPIO04
#define PIN_ENABLE              PIN_GPIO14


/* Commands */
#define CMD_NONE      "n"
#define CMD_ACCEL     "a"
#define CMD_STOP      "s"

#endif
