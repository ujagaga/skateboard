#ifndef BLDCM_H
#define BLDCM_H

#include <stdint.h>

extern void BLDCM_init(void);
extern void BLDCM_writeCmd(uint8_t val);
extern uint8_t BLDCM_getCurrentCmdVal( void );
extern void BLDCM_process(void);

#endif
