/*
 * gpio.ISR.h
 *
 *  Created on: Apr 18, 2016
 *      Author: Lukas
 */

#ifndef GPIO_ISR_H_
#define GPIO_ISR_H_

/* Funktions-Prototypen */
void gpio_init(void) ;
void MyEXTI9_5_IRQHandler(void);

/* Exportierte Daten*/
extern  uint32_t ISRCharacterCounter;

#endif /* GPIO_ISR_H_ */
