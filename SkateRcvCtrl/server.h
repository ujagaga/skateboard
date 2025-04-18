#ifndef SERVER_H
#define SERVER_H

#include "bldcMotor.h"

typedef struct{
  user_cmd_t cmd;
  uint8_t intensity;
}user_request_t;


extern void SERVER_init(void);
extern void SERVER_process(void);
extern user_request_t SERVER_getLastReceivedCommand(void);

#endif
