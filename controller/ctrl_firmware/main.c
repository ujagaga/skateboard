/*
 * main.c
 *
 *  Created on: Oct 20, 2016
 *      Author: ujagaga
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>

#include "main.h"

#define TX_ON()			do{ PORTD |= TX_PWR_MASK; }while(0)
#define TX_OFF()		do{ PORTD &= ~TX_PWR_MASK; }while(0)
#define TX_DATA_ON()	do{ PORTD |= TX_DATA_MASK; }while(0)
#define TX_DATA_OFF()	do{ PORTD &= ~TX_DATA_MASK; }while(0)

#define READ_CMD()		(PINB & (SW_SIREN_MASK | SW_UP_MASK | SW_OFF_MASK))


/* Initialize the hardware */
void HwInit( void )
{
	DDRD = TX_PWR_MASK | TX_DATA_MASK;			// set direction
	PORTD = ~(TX_PWR_MASK | TX_DATA_MASK);		// set pull up resistors for unused pins

	DDRB = 0;		// set port B as input
	PORTB = 0xFF;	// set pull up resistors on all pins

	DDRA = 0;
	PORTA = 3;

}

/* Main routine. This is a one time action at power on
 * and then nothing else until re-started */
int main( void )
{
	HwInit();

	TX_ON();
	_delay_ms(10);
	TX_DATA_ON();

	uint8_t cmd = READ_CMD();

	if(((cmd >> SW_OFF_PIN) & 1) == 0){
		/* Off key pressed */
		_delay_ms(20);
	}else if(((cmd >> SW_SIREN_PIN) & 1) == 0){
		/* Siren key pressed */
		_delay_ms(35);
	}else if(((cmd >> SW_UP_PIN) & 1) == 0){
		/* Up key pressed */
		_delay_ms(50);
	}

	TX_DATA_OFF();
	TX_OFF();

	while(1);	/* wait forever */

	return 0;
}
