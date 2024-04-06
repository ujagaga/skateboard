#include "config.h"
#include "wifi_connection.h"
#include "ws_client.h"

#define THREASHOLD        (100u)
#define CMD_READ_INTERVAL (200u)
#define LED_BLINK_COUNT   (9u)

#define POT_LOW           (0)
#define POT_MID           (1)
#define POT_HIGH          (2)
#define REPEAT_CMD_COUNT  (8)

static String lastCmd = "";
static volatile uint32_t readTimestamp = 0;
static uint8_t ledBlinkCounter = 0;
static int calibrateValue = 500;
static bool honkPressedFlag = false;
static uint8_t cmdCounter = 0;

uint8_t readCmd(){ 
  int32_t sensorValue = analogRead(PIN_ANALOG);

  if(sensorValue < (calibrateValue - THREASHOLD)){
    return POT_LOW;
  }else if(sensorValue > (calibrateValue + THREASHOLD)){
    return POT_HIGH;
  }
    
  return POT_MID;   
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
  
    String cmd = CMD_NONE;

    if(cmdVal == POT_HIGH){
      cmd = CMD_ACCEL + String("100");
    }else if(cmdVal == POT_LOW){
      cmd = CMD_STOP;
    }    
    
    if(!lastCmd.equals(cmd)){  
      WS_send(cmd);  
      lastCmd = cmd;
      cmdCounter = 0;
    }else{
      cmdCounter++;
      if(cmdCounter > REPEAT_CMD_COUNT){
        cmdCounter = 0;
        WS_send(cmd);
      }
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
