#include "config.h"
#include "wifi_connection.h"
#include "ws_client.h"

#define THREASHOLD        (5u)
#define CMD_READ_INTERVAL (200u)
#define LED_BLINK_COUNT   (9u)

static String lastCmd = "";
static bool fwdCmd = false;
static volatile uint32_t readTimestamp = 0;
static uint8_t ledBlinkCounter = 0;
static int calibrateValue = 508;
static int totalEstimatedVcc = 800; // This is re-calibrated at every start to account for battery drain
static bool honkPressedFlag = false;

uint8_t readCmd(){
  uint8_t percentage;
  
  int32_t sensorValue = analogRead(PIN_ANALOG);

  if(totalEstimatedVcc < sensorValue){
    totalEstimatedVcc = sensorValue;
    Serial.println(totalEstimatedVcc);
  }
  
  int fullRangeValue;  
  
  if(sensorValue > calibrateValue){
    fwdCmd = false;
    sensorValue -= calibrateValue;
    fullRangeValue = totalEstimatedVcc - calibrateValue; 
  }else{
    fwdCmd = true;
    sensorValue = calibrateValue - sensorValue;
    fullRangeValue = calibrateValue;
  }

  fullRangeValue -= 4; // Just to make sure we get 100 % when joystick position is maxed. 

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
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_ENABLE, OUTPUT);
  pinMode(PIN_HONK, INPUT_PULLUP);

  digitalWrite(PIN_LED, HIGH); 
  digitalWrite(PIN_LED2, LOW); 
  digitalWrite(PIN_ENABLE, HIGH); 

  Serial.begin(115200);

  delay(500);

  calibrateValue = analogRead(PIN_ANALOG);  
  
  WIFIC_init();    
  WIFIC_connectToScate();
  WS_init();
}


/* Main infinite while loop */
void loop(void) {

  /* Process honk button (connected to GND, so inverted.) */
  bool honkBtnPressed = !digitalRead(PIN_HONK);
  if(honkBtnPressed && !honkPressedFlag){
    /* Button just pressed */
    WS_send(CMD_HONK);
  }

  honkPressedFlag = honkBtnPressed;
    
  /* Process joystick command */
  if((millis() - readTimestamp) > CMD_READ_INTERVAL){    
    uint8_t cmdVal = readCmd();
  
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
      WS_send(cmd);  
      lastCmd = cmd;
    }  
    
    readTimestamp = millis();
    ledBlinkCounter++;    
  }

  /* Process LEDs */
  if(ledBlinkCounter > LED_BLINK_COUNT){
    ledBlinkCounter = 0;
  }  

  if(!WS_checkIfConnected() || (ledBlinkCounter == 0)){
    digitalWrite(PIN_LED, HIGH); 
    digitalWrite(PIN_LED2, LOW); 
  }else{
    digitalWrite(PIN_LED, LOW); 
    digitalWrite(PIN_LED2, HIGH); 
  }

  /* Process web socket client */
  WS_process();
 
}
