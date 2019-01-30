#ifndef BLDCM_H
#define BLDCM_H

#include <stdint.h>

typedef enum
{
  cmd_none = 0,
  cmd_accelerate,
  cmd_cruise,
  cmd_stop  
}user_cmd_t;

extern void BLDCM_init(void);
extern void BLDCM_process(void);
extern void BLDCM_setCommand(user_cmd_t cmd);
extern uint16_t BLDCM_getRelativeSpeed( void );

#endif
