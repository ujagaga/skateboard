/* This is a UART debug header for AtTiny2313 but should work with any 
 * AtTiny with UART HW. As this is for debug, only TX is configured. 
 * You may use this as you see fit :-) 
 * */

#ifndef _ATTINY_UART_DBG_H_
#define _ATTINY_UART_DBG_H_

#include <avr/io.h>

/* Configuration */
#define UART_DEBUG				/* comment to remove UART functionality */
#define USART_BAUDRATE 			(9600)		/* If the F_CPU of AtTiny is less than 8MHz, 9600baud is unreliable, so you should use less */
/* end of Configuration */

#define BAUD_PRESCALE 			(((( F_CPU / 16) + ( USART_BAUDRATE / 2) ) / ( USART_BAUDRATE ) ) - 1)
#define waitTxReady()			while (( UCSRA & (1 << UDRE ) ) == 0)

#ifdef UART_DEBUG
/* Initialize the UART HW for transmission only */
#define UART_init()				do{ UBRRL = (BAUD_PRESCALE & 0xFF);	\
									UBRRH = (BAUD_PRESCALE >> 8); 	\
									UCSRB = (1<<TXEN); 				\
								}while(0)
								
#define UART_send(oneByte)		do{ waitTxReady();UDR = (oneByte); }while(0)

/* Place this in a function or it will multiply with every call */
#define UART_print(string)		do{ char i=0; char data; 				\
									while(data != '\n'){ 				\
										data = string[i]; 				\
										UART_send(data);  				\
										i++;			}				\
								}while(0)

#else

#define UART_init()				/* to dust... */
#define UART_send(oneByte)		/* to dust... */
#define UART_print(string)     	/* to dust... */

#endif


#endif		/* end of _ATTINY_UART_DBG_H_ */
