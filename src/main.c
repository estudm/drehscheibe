/**
 *****************************************************************************
 * @defgroup	Project
 * @brief		Project related code
 * @{
 *
 * @file		main.c
 * @version		1.0
 * @date		2013-11-22
 * @author		rct1
 *
 * @brief		main.c FreeRTOS template
 *
 *****************************************************************************
 * @copyright
 * @{
 * Copyright &copy; 2013, Bern University of Applied Sciences.
 * All rights reserved.
 *
 * ##### GNU GENERAL PUBLIC LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 * @}
 *****************************************************************************
 */

/*----- Header-Files -------------------------------------------------------*/
#include <stdio.h>					/* Standard input and output			*/
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/
#include <uart.h>					/* CARME BSP UART port					*/

#include <FreeRTOS.h>				/* FreeRTOS								*/
#include <task.h>					/* FreeRTOS tasks						*/
#include <queue.h>					/* FreeRTOS queues						*/
#include <semphr.h>					/* FreeRTOS semaphores					*/
#include <memPoolService.h>			/* Memory pool manager service			*/

#include <carme_io1.h>
#include <carme_io2.h>
#include "gpio_ISR.h"
#include "motor_pwm.h"
#include "poti.h"
#include "buttons.h"

/*----- Macros -------------------------------------------------------------*/
#define INT_PRO_CHAR	(14)
#define SPACENMBR (35)			//Zeichen Nummer 35 auf Rad ist Leerzeichen//#define PRESCALER (14)#define PRIORITY_CONTROLTASK (4)
#define STACKSIZE_CONTROLTASK (128)
/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/
static void ControlTask(void *pvargs);
static void Serialtask(void *pvargs);

/*----- Data ---------------------------------------------------------------*/

SemaphoreHandle_t SemChanA;
SemaphoreHandle_t SemIndex;

QueueHandle_t QueueButtons;
QueueHandle_t QueueMotor;
QueueHandle_t QueueSPI;

uint8_t LedGPIO = 0;

/*----- Implementation -----------------------------------------------------*/
/**
 * @brief		main
 * @return		0 if success
 */
int main(void) {

	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* Create semaphore */
	SemChanA = xSemaphoreCreateCounting(1, 0);
	SemIndex = xSemaphoreCreateCounting(1, 0);

	/* Create Queue */
	QueueButtons = xQueueCreate(QUEUE_SIZE_BUTTON, sizeof(Msg_Buttons_t));

	CARME_IO1_Init();
	CARME_IO2_Init();
	InitISR(&SemIndex, &SemChanA);
	Motor_Init(4);

	xTaskCreate(ControlTask, "Control", STACKSIZE_CONTROLTASK, NULL,
			PRIORITY_CONTROLTASK, NULL);
	xTaskCreate(Serialtask, "Serial", 1024U, NULL, 4U, NULL);
	xTaskCreate(PotiTask, "PotiTask", STACKSIZE_POTITASK, NULL,
			PRIORITY_POTITASK, NULL);
	xTaskCreate(ButtonTask, "ButtonTask", STACKSIZE_BUTTONTASK, NULL,
			PRIORITY_BUTTONTASK, NULL);

	vTaskStartScheduler();
	for (;;) {
		Msg_Buttons_t ButtonMsg;
		if (xQueueReceive(QueueButtons,&ButtonMsg,1) == pdTRUE) {
			if ((ButtonMsg.ButtonStatus && 0x01) != 0) {
				LedGPIO = 0;
				//LED mit SPI
			} else {
				LedGPIO = 1;
				//LED mit GPIO
			}
		}
	}
	return 0;
}

/**
 * @brief		Blink the green LED on the CARME Module every second.
 * @param[in]	pvargs		Not used
 */
static void ControlTask(void *pvargs) {
	portTickType xLastWakeTime = xTaskGetTickCount();
	char buffer[32];
	uint32_t localCharacterCounter;
	uint8_t Cnt = 0;
	snprintf(buffer, sizeof(buffer), "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	for (;;) {
		while (buffer[Cnt] != 0) {
			localCharacterCounter = (buffer[Cnt] - 64) * INT_PRO_CHAR;
			portDISABLE_INTERRUPTS();
			CharacterCounter = localCharacterCounter;
			portENABLE_INTERRUPTS();
			vTaskDelayUntil(&xLastWakeTime, 1000u / portTICK_RATE_MS);
			Cnt++;
		}
		localCharacterCounter = 35 * INT_PRO_CHAR - 1;
		portDISABLE_INTERRUPTS();
		CharacterCounter = localCharacterCounter;
		portENABLE_INTERRUPTS();
		vTaskDelayUntil(&xLastWakeTime, 2000U / portTICK_RATE_MS);
		Cnt = 0;
	}
}

/**
 * @brief		Print welcome string to the standard output.
 * @param[in]	pvargs		Not used
 */
static void Serialtask(void *pvargs) {

	USART_InitTypeDef USART_InitStruct;
	USART_StructInit(&USART_InitStruct);
	USART_InitStruct.USART_BaudRate = 115200U;
	CARME_UART_Init(CARME_UART0, &USART_InitStruct);
	vTaskDelay(5U / portTICK_RATE_MS);
	printf("\033c"); /* Reset to initial state	*/
	printf("\033[2J"); /* Clear screen				*/
	printf("\033[?25l"); /* Cursor off				*/
	vTaskDelay(5 / portTICK_RATE_MS);

	printf("Welcome to CARME-M4 FreeRTOS\r\n");
	vTaskDelay(2000U / portTICK_RATE_MS);

	for (;;) {
		vTaskDelay(1000U);
	}
}

/**
 * @}
 */
