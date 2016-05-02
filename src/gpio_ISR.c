/**
 *****************************************************************************
 * @defgroup	GPIO-Interrupts
 * @{
 *
 * @file		gpio_ISR.c
 * @version		1.0
 * @date		02.05.2016
 * @author		kohll6, studm12
 *
 * @brief		Interruptroutine für Index und Kanal A
 *
 *****************************************************************************
 */


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


/*Anzahl Interrupts bis LED eingeschaltet wird.
  Wird von Control-Task aus gesezt*/
uint32_t ISRCharacterCounter=0;


/**
 * @brief Initialisiert Interrupts für Index und Kanal A.
 */
void gpio_init()
{
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


/**
 * @brief Kanal-A & Index Interrupt
 */
void MyEXTI9_5_IRQHandler(void) {
	static uint32_t Cnt=0;	/*Zählervariable*/

	/*Index Interrupt*/
	if (EXTI_GetITStatus(EXTI_Line8) != RESET)
	{
		Cnt=0;				/* Zähler zurücksetzen */
		EXTI_ClearITPendingBit (EXTI_Line8) ;/* Clear Interrupt*/
	}

	/*Kanal A Interrupt */
	if (EXTI_GetITStatus(EXTI_Line6) != RESET) {
		/*Zähler hat Sollzustand erreicht*/
		if(++Cnt==ISRCharacterCounter)
			{
				CARME_IO2_GPIO_OUT_Set(0x01);	/* LED einschalten*/
				CARME_IO2_GPIO_OUT_Set(0x00);	/* LED ausschalten */
			}
			EXTI_ClearITPendingBit (EXTI_Line6) ;	/*Clear Interrupt*/
		}
}

/**
 * @}
 */
