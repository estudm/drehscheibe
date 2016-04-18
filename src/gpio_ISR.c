#include <stdio.h>					/* Standard input and output			*/
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/
#include <FreeRTOS.h>				/* FreeRTOS								*/
#include <task.h>					/* FreeRTOS tasks						*/
#include <queue.h>					/* FreeRTOS queues						*/
#include <semphr.h>					/* FreeRTOS semaphores					*/
#include <carme_io1.h>
#include <carme_io2.h>
#include "gpio_ISR.h"

SemaphoreHandle_t SemGPIO;
SemaphoreHandle_t SemRSTCNTR;
uint8_t Counter=0;

void InitISR(void) {
	SemGPIO = xSemaphoreCreateCounting(1, 0);
	SemRSTCNTR = xSemaphoreCreateCounting(1, 0);

	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Connect EXTI7 to Kanal A */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource6);
	/* Configure EXTI7 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Connect EXTI8 to Index */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource8);
	/* Configure EXTI8 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
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
	if (EXTI_GetITStatus(EXTI_Line7) != RESET) {
		if (xSemaphoreGive(SemGPIO) == pdTRUE) {

		}
		Counter++;
		CARME_IO1_LED_Set(Counter,0xff);
	}
	if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
		if (xSemaphoreGive(SemRSTCNTR) == pdTRUE) {

		}

	}
}
