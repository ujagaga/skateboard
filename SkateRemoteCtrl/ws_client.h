#ifndef WSC_H
#define WSC_H

extern void WS_init(void);
extern void WS_send(String msg);
extern void WS_process(void);
extern bool WS_checkIfConnected();

#endif
