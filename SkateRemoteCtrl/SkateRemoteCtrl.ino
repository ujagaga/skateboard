#include "config.h"
#include "wifi_connection.h"
#include "ota.h"
#include "ws_client.h"

#define MAX_KEY_TEST_COUNT            (20u)
static bool otaModeFlag = false;

/* Define button type */
typedef enum{
  key_none,
  key_stop,
  key_accel,
  key_cruise,
  key_horn
}key_id_t;    

static bool keyPressed(key_id_t keyToTest){ 
  bool test[3] = {0};
  bool currentTest;
  bool testDone = false;
  int testCount = 0;
  bool testResult = false;

  /* Loop untill we get identical consecutive readings */
  do{ 
    
    switch(keyToTest){
      case key_stop:
        currentTest = (digitalRead(PIN_BREAK) == LOW);
        break;
      case key_accel:
        currentTest = (digitalRead(PIN_ACCEL) == LOW);
        break;
      case key_cruise:
        currentTest = (digitalRead(PIN_CRUISE) == LOW);
        break;
      case key_horn:
        currentTest = (digitalRead(PIN_HORN) == LOW);
        break;
      default:
        currentTest = false;       
    }

    test[0] = test[1]; 
    test[1] = test[2];
    test[2] = currentTest;     
      
    testCount++;
    
    if(testCount < 3){      
      delay(5);
    }else{
      if(testCount > MAX_KEY_TEST_COUNT){
        testDone = true; 
        testResult = false;       
      }else{        
        if((test[0] == test[1]) && (test[1] == test[2])){
          testDone = true; 
          testResult = test[0]; 
        }else{
          delay(5);
        }
      }
    }  
    
  }while(!testDone);

  return testResult;
}


/* Initialization */
void setup(void) {
  pinMode(PIN_ACCEL, INPUT_PULLUP);
  pinMode(PIN_CRUISE, INPUT_PULLUP);
  pinMode(PIN_BREAK, INPUT_PULLUP);
  pinMode(PIN_HORN, INPUT_PULLUP);

  WIFIC_init();
  
  if( keyPressed(key_stop)){
    otaModeFlag = true;
    OTA_init();
    WIFIC_connectToMainNetwork();
  }else{
    WIFIC_connectToScate();
    WS_init();
  }
}


/* Main infinite while loop */
void loop(void) {
  if(otaModeFlag){
    OTA_process();
  }else{
    /* Process buttons */
    if( keyPressed(key_stop)){
      WS_send(CMD_STOP);     
    }else if(keyPressed(key_accel)){
      WS_send(CMD_ACCEL);  
    }else if( keyPressed(key_cruise)){
      WS_send(CMD_CRUISE);  
    }else if( keyPressed(key_horn)){
      WS_send(CMD_HORN);  
    }else{
      WS_send(CMD_NONE); 
    }  

    WS_process();
  }
}
