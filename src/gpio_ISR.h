/*
 * gpio.ISR.h
 *
 *  Created on: Apr 18, 2016
 *      Author: Lukas
 */

#ifndef GPIO_ISR_H_
#define GPIO_ISR_H_

void InitISR(void);
void MyEXTI9_5_IRQHandler(void);

extern SemaphoreHandle_t SemGPIO;

#endif /* GPIO_ISR_H_ */
