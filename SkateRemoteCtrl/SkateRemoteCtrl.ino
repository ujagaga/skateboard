#include "config.h"
#include "wifi_connection.h"
#include "ws_client.h"

#define THREASHOLD        (5u)
#define CMD_READ_INTERVAL (200u)
#define LED_BLINK_COUNT   (6u)

static String lastCmd = "";
static bool fwdCmd = false;
static volatile uint32_t readTimestamp = 0;
static uint8_t ledBlinkCounter = 0;
static int calibrateValue = 508;


void goToSleep(){

  digitalWrite(PIN_LED, LOW); 
  digitalWrite(PIN_ENABLE, LOW); 
  
  ESP.deepSleep(0);
}

uint8_t readCmd(){
  uint8_t percentage;
  
  int32_t sensorValue = analogRead(PIN_ANALOG);
  int fullRangeValue;  
  
  if(sensorValue > calibrateValue){
    fwdCmd = false;
    sensorValue -= calibrateValue;
    fullRangeValue = 1020 - calibrateValue;
  }else{
    fwdCmd = true;
    sensorValue = calibrateValue - sensorValue;
    fullRangeValue = calibrateValue - 4;
  }

  if(sensorValue < THREASHOLD){
    percentage = 0;
  }else{
    percentage = (sensorValue * 100) / fullRangeValue;

    if(percentage > 100){
      percentage = 100;
    }
  }  
  
  return percentage;
}


/* Initialization */
void setup(void) {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_ENABLE, OUTPUT);

  digitalWrite(PIN_LED, HIGH); 
  digitalWrite(PIN_ENABLE, HIGH); 

  Serial.begin(115200);

  delay(500);

  calibrateValue = analogRead(PIN_ANALOG);  
  
  WIFIC_init();    
  WIFIC_connectToScate();
//  WIFIC_connectToMainNetwork();
  WS_init();
}


/* Main infinite while loop */
void loop(void) {
  uint8_t cmdVal;

  if((millis() - readTimestamp) > CMD_READ_INTERVAL){
    cmdVal = readCmd();
  
    String cmd;

    if(cmdVal == 0){
      cmd = CMD_NONE;
    }else if(fwdCmd){
      cmd = CMD_ACCEL;
    }else{
      cmd = CMD_STOP;
    }

    if(cmdVal < 10){
      cmd += "00";
    }else if(cmdVal < 100){
      cmd += "0";
    }
    
    cmd += String(cmdVal);

    if(!lastCmd.equals(cmd)){
    
      Serial.println(cmd);
      WS_send(cmd);  

      lastCmd = cmd;
    }  
    
    readTimestamp = millis();
    ledBlinkCounter++;    
  }


  if(ledBlinkCounter > LED_BLINK_COUNT){
    ledBlinkCounter = 0;
  }
  
  if(ledBlinkCounter == 0){
    digitalWrite(PIN_LED, HIGH); 
  }else if(WIFIC_checkIfConnected()){
    digitalWrite(PIN_LED, LOW); 
  }
  
  WS_process();
 
}
