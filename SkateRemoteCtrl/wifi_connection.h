#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

#include <Ethernet.h>

extern void WIFIC_init( void );
extern void WIFIC_connectToScate( void );
extern void WIFIC_connectToMainNetwork( void );
extern char* WIFIC_getDeviceName( void );

#endif
