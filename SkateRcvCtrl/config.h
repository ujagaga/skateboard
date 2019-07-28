#ifndef SKATECTRL_OTAA_H
#define SKATECTRL_OTAA_H

#include <ESP8266WiFi.h>

#define D0    16
#define D1    5
#define D2    4
#define D3    0
#define D4    2
#define D5    14
#define D6    12
#define D7    13
#define D8    15

/* This devices hotsopt credentials */
#define AP_NAME                 "ujagaga_skate"
#define AP_PASS                 "SkatePass123"

#define TCP_PORT                (4213u)  
#define CMD_TIMEOUT             (350u)
#define EXEC_REFRESH_TIMEOUT    (100u)
#define OTAA_UPDATE_TIMEOUT     (10000u)

#define PIN_EL                  (D4) 
#define PIN_TAHO                (D6) 
#define PIN_CMD                 (D2) 
#define PIN_DIR                 (D1) 
#define PIN_SPEAKER             (D8)
#define PIN_BRAKE               (D7) 

#define MINIMUM_SPEED_RPM       (120) /* 2Rev per second => ((90mm*3.14*2)/1000)mps ~ 2kmph */
#define HAL_TICKS_PER_REV       (40) 

#define ACCELERATE_INCREMENT    (10)   
#define BREAK_INCREMENT         (50)
#define BREAK_VAL_1             (200)
#define BREAK_VAL_2             (500)

#define SPEED_LIMIT             (30u)   // kmph

/* If in test mode, mooving() will always show true. */
//#define TEST_MODE         
#endif
