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
#define D8    15  // Do not use. ESP12 connected to GND.

/* This devices hotsopt credentials */
#define AP_NAME                 "ujagaga_skate"
#define AP_PASS                 "SkatePass123"

#define TCP_PORT                (4213u)  
#define CMD_TIMEOUT             (350u)
#define EXEC_REFRESH_TIMEOUT    (100u)

#define PIN_CMD                 (5)  // Do not use D2 for PWM. It is close to the oscillator and causes connection drop.
#define PIN_BRAKE               (14) 
#define PIN_HONK                (4)
#define PIN_FRONT_LIGHT         (12)
#define PIN_REAR_LIGHT          (13)

#define ACCELERATE_INCREMENT    (10)   
#define SPEED_LIMIT             (1023)  
// #define REVERSE_SPEED_POLARITY
    
#endif
