#ifndef SERVER_H
#define SERVER_H

#include "bldcMotor.h"

extern void SERVER_init(void);
extern void SERVER_process(void);
extern user_cmd_t SERVER_getLastReceivedCommand(void);

#endif
