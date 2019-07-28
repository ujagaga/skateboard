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
#define BATT_FULL_COUNT         (800u)    /* Measuring of the ADC at battery full voltage. */
#define BATT_LOW_COUNT          (700u)    /* Measuring of the ADC at battery low voltage. */
#define BATT_MIN_COUNT          (100u)    /* Minimum voltage to consider the battery connected */

/* ESP-01 pin defines */
#define PIN_UARTTX              (1u)
#define PIN_UARTRX              (3u)
#define PIN_GPIO0               (0u)
#define PIN_GPIO2               (2u)

/* Connections */
#define PIN_ACCEL               PIN_UARTTX 
#define PIN_CRUISE              PIN_GPIO0
#define PIN_BREAK               PIN_UARTRX
#define PIN_HORN                PIN_GPIO2


/* Commands */
#define CMD_NONE      "n"
#define CMD_ACCEL     "a"
#define CMD_CRUISE    "c"
#define CMD_STOP      "s"
#define CMD_HORN      "h"

#endif
