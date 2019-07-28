#include "config.h"
#include "current.h"



const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0
static unsigned long readTimestamp = 0;
static int sensorValue = 0;

void CURR_process() 
{
  if((millis() - readTimestamp) > 50){
    int readVal = analogRead(analogInPin);      
    sensorValue = MAX_VAL - readVal;    
    readTimestamp = millis();
  }
}

int CURR_getVal()
{
  return sensorValue;
}
