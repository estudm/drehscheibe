/*
 * gpio_ISR.c
 *
 *  Created on: Apr 18, 2016
 *      Author: Lukas
 */


#include <semphr.h>					/* FreeRTOS semaphores					*/
#include <carme_io1.h>
#include "gpio_ISR.h"

void InitISR(void) {
	SemGPIO = xSemaphoreCreateCounting(1, 0);

	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Connect EXTI7 to C7 (Button0) */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG,EXTI_PinSource6);
	/* Configure EXTI7 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	/* Enable and set EXTI9_5 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void MyEXTI9_5_IRQHandler(void) {
	/* Button T0 interrupt on Port C Pin ( Line ) 7 */
	if(EXTI_GetITStatus(EXTI_Line7) != RESET)
	{
		if(xSemaphoreGive(SemGPIO)==pdTrue)
		{

		}
	}
}
