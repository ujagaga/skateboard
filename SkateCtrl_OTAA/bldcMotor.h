#ifndef BLDCM_H
#define BLDCM_H

#include <stdint.h>

typedef enum
{
  cmd_none,
  cmd_accelerate,
  cmd_stop,
  cmd_cruiseCtrlOn,
  cmd_cruiseCtrlOff
}user_cmd_t;

extern void BLDCM_init(void);
extern void BLDCM_writeCmd(uint8_t val);
extern uint8_t BLDCM_getCurrentCmdVal( void );
extern void BLDCM_process(void);
extern bool BLDCM_setCommand(user_cmd_t cmd);

#endif
