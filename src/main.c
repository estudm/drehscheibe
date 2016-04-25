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
#include "spi.h"
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
QueueHandle_t QueueButtons;
QueueHandle_t QueueMotor;
QueueHandle_t QueuePoti;
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


	/* Create Queues */
	QueueButtons = xQueueCreate(QUEUE_SIZE_BUTTON, sizeof(Msg_Buttons_t));
	QueuePoti=xQueueCreate(QUEUE_SIZE_POTI,sizeof(Msg_Poti_t));
	QueueMotor=xQueueCreate(QUEUE_SIZE_MOTORTASK,sizeof(Msg_Motor_t));
	QueueSPI=xQueueCreate(QUEUE_SIZE_SPI,sizeof(Msg_SPI_t));

	CARME_IO1_Init();
	CARME_IO2_Init();
	InitISR();

	xTaskCreate(ControlTask, "Control", STACKSIZE_CONTROLTASK, NULL,PRIORITY_CONTROLTASK, NULL);
	xTaskCreate(Serialtask, "Serial", 1024U, NULL, 4U, NULL);
	xTaskCreate(PotiTask, "PotiTask", STACKSIZE_POTITASK,(void *) &QueuePoti,	PRIORITY_POTITASK, NULL);
	xTaskCreate(ButtonTask, "ButtonTask", STACKSIZE_BUTTONTASK, (void *)&QueueButtons,PRIORITY_BUTTONTASK, NULL);
	xTaskCreate(MotorTask, "Motor Task", STACKSIZE_MOTORTASK, (void *)&QueueMotor,PRIORITY_MOTORTASK, NULL);
	xTaskCreate(SPITask,"SPI Task",STACKSIZE_SPITASK,(void*) &QueueSPI,PRIORITY_SPITASK,NULL);
	vTaskStartScheduler();
	for (;;) {

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
	snprintf(buffer, sizeof(buffer), "HALLO");
	for (;;)
	{
		Msg_Buttons_t 	ButtonMsg;
		Msg_Poti_t		PotiMsg;
		Msg_Motor_t		MotorMsg;
		Msg_SPI_t		SPIMsg;
		if(xQueueReceive(QueuePoti,&PotiMsg,0)==pdTRUE)
		{
			MotorMsg.PWMValue=PotiMsg.PotiVal;
			MotorMsg.dir=CARME_IO2_PWM_NORMAL_DIRECTION;
			if(xQueueSend(QueueMotor,&MotorMsg,0)==pdTRUE)
			{

			}
		}


		if (xQueueReceive(QueueButtons,&ButtonMsg,0) == pdTRUE)
		{
			if(ButtonMsg.ButtonStatus&0x01)						//Schalter0 ein
			{
				SPIMsg.flashtime=ButtonMsg.ButtonStatus;		//LED On-Zeit kann mit Schalter eingestellt werden
			}
			else												//Schalter0 aus
			{
				SPIMsg.flashtime=255-255*MotorMsg.PWMValue/100;//LED On-Zeit wird durch PWM-Wert bestimmt
			}
			if(xQueueSend(QueueSPI,&SPIMsg,0)==pdTRUE)
			{

			}

		}



/* Character Handling *******************************************************************/
		if(Cnt==0)
		{
			localCharacterCounter = (buffer[Cnt] - 65) * INT_PRO_CHAR+1;
			portDISABLE_INTERRUPTS();
			CharacterCounter = localCharacterCounter;
			portENABLE_INTERRUPTS();
			xLastWakeTime=xTaskGetTickCount();
			Cnt++;
		}
		else
		{
			if(xTaskGetTickCount()-xLastWakeTime>1000)
			{

				if (buffer[Cnt] != 0)
				{
					localCharacterCounter = (buffer[Cnt] - 65) * INT_PRO_CHAR+1;
					portDISABLE_INTERRUPTS();
					CharacterCounter = localCharacterCounter;
					portENABLE_INTERRUPTS();
					xLastWakeTime=xTaskGetTickCount();
					Cnt++;
				}
				else
				{
					localCharacterCounter = 36 * INT_PRO_CHAR - 1;
					portDISABLE_INTERRUPTS();
					CharacterCounter = localCharacterCounter;
					portENABLE_INTERRUPTS();
					if(xTaskGetTickCount()-xLastWakeTime>2000)
					{
						Cnt = 0;
					}

				}
			}
		}
		vTaskDelay(10);
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
