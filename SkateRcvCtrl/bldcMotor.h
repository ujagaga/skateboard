#ifndef BLDCM_H
#define BLDCM_H

#include <stdint.h>

typedef enum
{
  cmd_none = 0,
  cmd_accelerate,
  cmd_stop,
  cmd_undefined
}user_cmd_t;

extern void BLDCM_init(void);
extern void BLDCM_process(void);
extern void BLDCM_setSpeed(uint32_t val);
extern uint16_t BLCMD_getSpeed(void);
extern uint16_t BLCMD_getCmdVoltage();
extern void BLDCM_setTargetSpeed(uint8_t percent);

#endif
