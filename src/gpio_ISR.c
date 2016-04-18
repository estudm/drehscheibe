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

static SemaphoreHandle_t SemIndex;
static SemaphoreHandle_t SemChanA;
uint32_t CharacterCounter=0;
void InitISR(SemaphoreHandle_t *PSemIndex,SemaphoreHandle_t *PSemChanA )
{
	SemChanA = *PSemChanA;
	SemIndex = *PSemIndex;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Connect EXTI8 to Index */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource8);
	/* Configure EXTI8 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);


	/* Connect EXTI6 to Kanal A */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource6);
	/* Configure EXTI6 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
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
	static uint32_t Cnt=0;
	/*Index Interrupt*/
	if (EXTI_GetITStatus(EXTI_Line8) != RESET) {

		Cnt=0;
		/* Clear Interrupt and call Interrupt Service Routine */
		EXTI_ClearITPendingBit (EXTI_Line8) ;

	}
	/*Kanal A Interrupt */
	if (EXTI_GetITStatus(EXTI_Line6) != RESET) {

		if(++Cnt==CharacterCounter)
			{
				CARME_IO2_GPIO_OUT_Set(0x01);
				CARME_IO2_GPIO_OUT_Set(0x00);
			}
			EXTI_ClearITPendingBit (EXTI_Line6) ;
		}
}
