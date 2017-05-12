/*
 * main.h
 *
 *  Created on: Oct 24, 2016
 *      Author: rada
 */

#ifndef MAIN_H_
#define MAIN_H_

/* Configuration */
#define TX_PWR_PIN		(PD0)
#define TX_DATA_PIN		(PD1)

#define SW_SIREN_PIN	(PB1)
#define SW_UP_PIN		(PB3)
#define SW_OFF_PIN		(PB4)
/* End of configuration */

#define TX_PWR_MASK		(1 << TX_PWR_PIN)
#define TX_DATA_MASK	(1 << TX_DATA_PIN)
#define SW_SIREN_MASK	(1 << SW_SIREN_PIN)
#define SW_UP_MASK		(1 << SW_UP_PIN)
#define SW_OFF_MASK		(1 << SW_OFF_PIN)

#endif /* MAIN_H_ */
