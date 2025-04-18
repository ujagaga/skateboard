#include "peripherals.h"
#include "config.h"

/* Light will run at lower intensity with periodic bursts. 
 * The rear light is offset to reduce maximum power consumption. 
 *              _           _
 * Front: _____| |_________| |______
 *                 _           _
 * Rear:  ________| |_________| |______
 */
#define LIGHT_NORMAL_INTENSITY          (100)
#define LIGHT_MAX_INTENSITY             (1023)
#define LIGHT_MAX_DURATION              (100)
#define LIGHT_NORMAL_DURATION           (2000ul)

#define HONK_DURATION                   (900ul)

static uint32_t honkTimestamp = 0;
static uint32_t lightTimestamp = 0;


void PERIPHERALS_init(void)
{
  pinMode(PIN_HONK, OUTPUT);
  digitalWrite(PIN_HONK, LOW); 

  pinMode(PIN_FRONT_LIGHT, OUTPUT);
  analogWrite(PIN_FRONT_LIGHT, 0);
  
  pinMode(PIN_REAR_LIGHT, OUTPUT);
  analogWrite(PIN_REAR_LIGHT, 0);   
}

void PERIPHERALS_process(void){
    uint32_t ts = millis();

    // Process honk
    uint32 t_HDelta = ts - honkTimestamp;
    if(honkTimestamp > 0){
        if(t_HDelta > HONK_DURATION){
          honkTimestamp = 0;
          digitalWrite(PIN_HONK, LOW); 
        }else if(t_HDelta > (2 * HONK_DURATION / 3)){
          digitalWrite(PIN_HONK, HIGH); 
        }else if(t_HDelta > (HONK_DURATION / 3)){
          digitalWrite(PIN_HONK, LOW); 
        }else{
          digitalWrite(PIN_HONK, HIGH); 
        }
    }else{
        digitalWrite(PIN_HONK, LOW); 
    }

    // Process light
    uint32 t_LDelta = ts - lightTimestamp;
    int fLight = LIGHT_NORMAL_INTENSITY;
    int rLight = LIGHT_NORMAL_INTENSITY;

    if(t_LDelta < LIGHT_NORMAL_DURATION){

    }else if(t_LDelta < (LIGHT_NORMAL_DURATION + LIGHT_MAX_DURATION)){
        fLight = LIGHT_MAX_INTENSITY;

    }else if(t_LDelta < (LIGHT_NORMAL_DURATION + 2 * LIGHT_MAX_DURATION)){
        rLight = LIGHT_MAX_INTENSITY;  
        
    }else{
        lightTimestamp = ts;
    }

    analogWrite(PIN_FRONT_LIGHT, fLight);
    analogWrite(PIN_REAR_LIGHT, rLight);
}

void PERIPHERALS_honk(void)
{
    honkTimestamp = millis();
}
