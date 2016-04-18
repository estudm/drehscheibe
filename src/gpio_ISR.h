/*
 * gpio.ISR.h
 *
 *  Created on: Apr 18, 2016
 *      Author: Lukas
 */

#ifndef GPIO_ISR_H_
#define GPIO_ISR_H_

void InitISR(SemaphoreHandle_t *PSemIndex,SemaphoreHandle_t *PSemChanA ) ;
void MyEXTI9_5_IRQHandler(void);

extern  uint32_t CharacterCounter;

#endif /* GPIO_ISR_H_ */
