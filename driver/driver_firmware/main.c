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
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include "attiny_uart_dbg.h"

/*--------- Configuration ----------------------*/
#define WEIGHT_PIN			(PA1)	// input
#define REFERENCE_PIN		(PA0)	// input
#define RX_DATA_PIN			(PD0)	// input
#define SIREN_PIN			(PD5)	// output
#define LED_PIN				(PB4)	// output
#define MOTOR_PIN			(PB2)	// output

#define LIGHT_PWM			(100u)	// number of 8 bit counter where the light goes on
#define CMD_0				(10u)	// minimum command length in ms (the first one)
#define CMD_1				(15u)	// second command length in ms
#define CMD_2				(20u)	// third command length in ms
#define CMD_TOLERANCE		(2u)	// command length tolerance
#define WEIGHT_THREASHOLD_MULTIPLIER	(25)

//#define DEBUG_WEIGHT
//#define DEBUG_COMMAND
/*---------- End of configuration --------------*/

#define WEIGHT_MASK			(1 << WEIGHT_PIN)
#define REFERENCE_MASK		(1 << REFERENCE_PIN)
#define RX_DATA_MASK		(1 << RX_DATA_PIN)
#define SIREN_MASK			(1 << SIREN_PIN)
#define LED_MASK			(1 << LED_PIN)
#define MOTOR_MASK			(1 << MOTOR_PIN)
#define weight_read()		((PINA & WEIGHT_MASK) == WEIGHT_MASK)
#define reference_read()	((PINA & REFERENCE_MASK) == REFERENCE_MASK)
#define data_read()			((PIND & RX_DATA_MASK) == RX_DATA_MASK)
#define siren_on()			do{ PORTD |= SIREN_MASK; }while(0)
#define siren_off()			do{ PORTD &= ~SIREN_MASK; }while(0)
#define LED_on()			do{ PORTB |= LED_MASK; }while(0)
#define LED_off()			do{ PORTB &= ~LED_MASK; }while(0)
#define LED_tgl()			do{ PINB = LED_MASK; }while(0)
#define motor_on()			do{ PORTB |= MOTOR_MASK; }while(0)
#define motor_off()			do{ PORTB &= ~MOTOR_MASK; }while(0)
#define stop()				do{ speed_idx = 0; set_target_speed(); }while(0)
#define light_on()			do{ light_flag = true; }while(0)
#define light_off()			do{ light_flag = false; }while(0)
#define set_speed(x)		do{ OCR0A = (x); }while(0)
#define set_target_speed()	do{ target_speed = pwm[speed_idx]; }while(0)
#define speed_up()			do{ if(speed_idx < max_speed_idx){ speed_idx++; } set_target_speed(); }while(0)
#define max_speed_idx  		(6u)	/* pwm[] max index */
#define rxBufSize			(16)
#define rxBufMask			(rxBufSize - 1)

/*--------- Forward declarations ---------------*/
void update_speed(void);
void update_light(void);
void update_comms(void);


/*--------- Global variables ---------------*/
const uint8_t pwm[] = { 0, 105, 135, 165, 195, 225, 255 };
bool light_flag = false;
uint8_t target_speed = 0;
volatile uint8_t current_speed = 0;
volatile uint16_t speed_count = 0;
volatile uint8_t light_count = 0;
volatile uint8_t comm_on_counter = 0;
volatile uint8_t comm_off_counter = 0;
volatile uint8_t rxBuf[16];
volatile uint8_t rxRdIdx = 0;
volatile uint8_t rxWrIdx = 0;

#define isRxEmpty()	(rxWrIdx == rxRdIdx)
#define isRxFull()	(((rxWrIdx - rxRdIdx) & rxBufMask) > (rxBufSize - 2))
#define rxHasData()	(((rxWrIdx - rxRdIdx) & rxBufMask) > 0)

/* Timer1 ISR. Triggers at 1ms */
ISR(TIMER1_COMPA_vect){
	update_comms();
//	update_speed();
//	update_light();
}


void update_light(void){
	if(light_flag){
		light_count++;
	}else{
		light_count = 0;
	}

	if(light_count > LIGHT_PWM ){
		LED_on();
	}else{
		LED_off();
	}
}

void update_speed(void){
	speed_count++;

	if(speed_count == 50){
		/* 0.1s*/
		speed_count = 0;

		if(current_speed < target_speed){
			current_speed += 1;
		}else{
			current_speed = target_speed;
		}

		uint8_t result = ~current_speed;	// due to inverted PWM output
		set_speed(result);
	}
}

void update_comms(void){
	if(data_read()){
		if(comm_off_counter != 0){
			if(!isRxFull()){
				rxBuf[(rxWrIdx & rxBufMask)] = comm_off_counter;
				rxWrIdx++;
			}
			comm_off_counter = 0;
		}
		comm_on_counter++;
	}else{
		if(comm_on_counter != 0){
			if(!isRxFull()){
				rxBuf[(rxWrIdx & rxBufMask)] = comm_on_counter;
				rxWrIdx++;
			}
			comm_on_counter = 0;
		}
		comm_off_counter++;
	}
}

bool getRx( uint8_t* data ){

	if(rxHasData()){
		*data = rxBuf[(rxRdIdx & rxBufMask)];
		return true;
	}

	return false;
}

/* Initializes the hardware */
void HwInit( void )
{
	//reset watchdog. This must be the first thing done
	wdt_reset();
	//Start watchdog timer with 4s prescaller
	WDTCSR = (1<<WDIE)|(1<<WDE)|(1<<WDP3);
	wdt_enable(WDTO_4S);

	DDRD = SIREN_MASK;						// set direction on PORTD
	PORTD = ~(SIREN_MASK | RX_DATA_MASK);	// set pull up resistors for unused pins

	DDRB = LED_MASK | MOTOR_MASK;			// set direction on PORTB
	PORTB = ~(LED_MASK | MOTOR_MASK);		// set pull up resistors for unused pins

	DDRA = WEIGHT_PIN | REFERENCE_PIN;		// set direction on PORTA
	PORTA = 0;

	/* Setup timer0 for PWM */
	TCCR0A = (1 << COM0A1) | (1 << WGM00);  // phase correct PWM mode
	OCR0A  = 0xFF;                          // initial PWM pulse width
	TCCR0B = (1 << CS01);   				// clock source = CLK/8, start PWM

	/* Setup Timer1 for timing */
	OCR1A = 8000;         					// 8000 number to count to for 1ms
	TCCR1A = 0;             				// CTC mode
	TCCR1B = (1 << WGM12) | (1 << CS10);	// CTC mode, clk src = clk/1, start timer
	TIMSK |= (1 << OCIE1A);					// enable compare interrupt

	UART_init();

	sei();
}


/* Sounds the siren for 0,5 seconds */
void siren(void){
	uint8_t i, j;

	for(i=0; i<10; i++){
		for(j=0; j<10; i++){
			siren_on();
			_delay_ms(1);
			siren_off();
			_delay_ms(1);
		}
		_delay_ms(20);
	}
}

/* Detects if there is a driver on the scateboard */
bool weight_detect(void){
	uint8_t charge_time = 0;
	uint8_t threashold;

	/* compare pin is output and 0 */
	DDRA &= ~REFERENCE_PIN;	// set pin as input
	/* measure charge time */
	do{
		charge_time++;
	}while(reference_read());

	/* Clear compare */
	DDRA |= REFERENCE_PIN;	// set pin as output
	PORTA &= ~REFERENCE_PIN;	// clear pin

	threashold = (uint8_t)(((uint16_t)charge_time * WEIGHT_THREASHOLD_MULTIPLIER)/10);

	charge_time = 0;
	/* weight pin is output and 0 */
	DDRA &= ~WEIGHT_MASK;	// set pin as input
	/* measure charge time */
	do{
		charge_time++;
	}while(weight_read());

	/* Clear weight */
	DDRA |= WEIGHT_MASK;	// set pin as output
	PORTA &= ~WEIGHT_MASK;	// clear pin
	_delay_ms(10);			// keep it low at least 10ms so that the capacitors discharge

	if(charge_time > threashold){
		return true;
	}

	return false;
}


#ifdef DEBUG_WEIGHT
/* Just for measuring. Remove later.
 * Sends measured charge time of reference capacity after 0xAA,
 * measured sensor capacity after 0xBB,
 * ration times 10 after 0xCC*/
void weight_measure(void){
	uint8_t charge_time1 = 0;
	uint8_t charge_time2 = 0;

	UART_send(0xAA);
	/* compare pin is output and 0 */
	DDRA &= ~REFERENCE_PIN;	// set pin as input
	/* measure charge time */
	do{
		charge_time1++;
	}while(reference_read());

	UART_send(charge_time1);

	/* Clear compare */
	DDRA |= REFERENCE_PIN;	// set pin as output
	PORTA &= ~REFERENCE_PIN;	// clear pin

	UART_send(0xBB);
	/* weight pin is output and 0 */
	DDRA &= ~WEIGHT_MASK;	// set pin as input
	/* measure charge time */
	do{
		charge_time2++;
	}while(weight_read());

	UART_send(charge_time2);

	/* Clear weight */
	DDRA |= WEIGHT_MASK;	// set pin as output
	PORTA &= ~WEIGHT_MASK;	// clear pin

	uint8_t ratio = (uint8_t)((uint16_t)(charge_time2 * 10) / (uint16_t)(charge_time1 * 10));

	UART_send(0xCC);
	UART_send(ratio);

	_delay_ms(100);			// keep it low at least 10ms so that the capacitors discharge

}
#endif

/* Main routine. This is a one time action at power on
 * and then nothing else until re-started */
int main( void )
{
	uint8_t speed_idx = 0;
	uint8_t data;

	HwInit();


#ifdef DEBUG_WEIGHT
	for(;;){
		weight_measure();
		_delay_ms(400);
	}
#endif

	LED_on();
	_delay_ms(300);
	LED_off();

	for(;;){	// Infinite main loop
		wdt_reset();

		if(getRx(&data)){

		}


	}




//	for(;;){	// Infinite main loop
//		wdt_reset();
//
//		if(weight_detect()){
//
//			light_on();
//
//			/* Check command */
//			if((cmd_len > (CMD_0 - CMD_TOLERANCE)) && (cmd_len < (CMD_0 + CMD_TOLERANCE))){
//				/* first command */
//				stop();
//			}else if((cmd_len > (CMD_1 - CMD_TOLERANCE)) && (cmd_len < (CMD_1 + CMD_TOLERANCE))){
//				/* Second command */
//				cmd_len = 0;		// Siren might take longer, so do not block execution
//				siren();
//			}else if((cmd_len > (CMD_2 - CMD_TOLERANCE)) && (cmd_len < (CMD_2 + CMD_TOLERANCE))){
//				/* third command */
//				speed_up();
//			}
//		}else{
//			light_off();
//			stop();
//		}
//
//		cmd_len = 0;
//	}

	return 0;
}
