/**
 *****************************************************************************
 * @defgroup	RT-Modell
 * @brief		Project related code
 * @{
 *
 * @file		main.c
 * @version		1.0
 * @date		02.05.2016
 * @author		kohll6, studm12
 *
 * @brief		main.c Main-Source File, with implemented Control Task
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
#include "rs232.h"
#include "display.h"
/*----- Macros -------------------------------------------------------------*/
#define INT_PRO_CHAR	(14)	//Anzahl Interrupts pro Zeichen (500 Int/U)
#define SPACENMBR (35)			//Zeichen Nummer 35 auf Rad ist Leerzeichen#define PRIORITY_CONTROLTASK (4)
#define STACKSIZE_CONTROLTASK (128)
/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/
static void ControlTask(void *pvargs);

/*----- Data ---------------------------------------------------------------*/
QueueHandle_t QueueButtons;
QueueHandle_t QueueMotor;
QueueHandle_t QueuePoti;
QueueHandle_t QueueSPI;
QueueHandle_t QueueUart;
QueueHandle_t QueueDisplay;

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
	QueueUart=xQueueCreate(QUEUE_SIZE_UART,sizeof(Msg_Uart_t));
	QueueDisplay=xQueueCreate(QUEUE_SIZE_LCD,sizeof (Msg_LCD_t));

	CARME_IO1_Init();
	CARME_IO2_Init();
	uart_init(&QueueUart);
	InitISR();


	xTaskCreate(ControlTask, "Control", STACKSIZE_CONTROLTASK, NULL,PRIORITY_CONTROLTASK, NULL);
	xTaskCreate(PotiTask, "PotiTask", STACKSIZE_POTITASK,(void *) &QueuePoti,	PRIORITY_POTITASK, NULL);
	xTaskCreate(ButtonTask, "ButtonTask", STACKSIZE_BUTTONTASK, (void *)&QueueButtons,PRIORITY_BUTTONTASK, NULL);
	xTaskCreate(MotorTask, "Motor Task", STACKSIZE_MOTORTASK, (void *)&QueueMotor,PRIORITY_MOTORTASK, NULL);
	xTaskCreate(SPITask,"SPI Task",STACKSIZE_SPITASK,(void*) &QueueSPI,PRIORITY_SPITASK,NULL);
	xTaskCreate(LCDTask, "LCD Task",STACKSIZE_LCDTASK, (void*) &QueueDisplay,PRIORITY_LCDTASK,NULL);
	vTaskStartScheduler();
	for (;;) {

	}
	return 0;
}

/**
 * @brief
 * @param		pvargs not used
 */
static void ControlTask(void *pvargs) {
	portTickType xLastWakeTime = xTaskGetTickCount();
	char buffer[UART_MAXTEXTLENGTH]={0};
	uint32_t localCharacterCounter;
	uint8_t Cnt = 0;
	uint32_t i=0;
	Msg_Buttons_t 	ButtonMsg;
	Msg_Poti_t		PotiMsg;
	Msg_Motor_t		MotorMsg;
	Msg_SPI_t		SPIMsg;
	Msg_Uart_t		UartMsg;
	Msg_LCD_t		LCDMsg;


	snprintf(buffer, sizeof(buffer), "HALLO");
	snprintf(LCDMsg.MsgString,sizeof(LCDMsg.MsgString),"HALLO");
	printf("\r\n Neuen Text eingeben (max %u Zeichen):", UART_MAXTEXTLENGTH);


	for (;;)
	{
		if(xQueueReceive(QueuePoti,&PotiMsg,0)==pdTRUE)
		{
			MotorMsg.PWMValue=PotiMsg.PotiVal;
			MotorMsg.dir=CARME_IO2_PWM_NORMAL_DIRECTION;
			LCDMsg.PotiStatus=PotiMsg.PotiVal;
			if(xQueueSend(QueueMotor,&MotorMsg,0)==pdTRUE)
			{

			}
			if(xQueueSend(QueueDisplay,&LCDMsg,0)==pdTRUE)
			{

			}
		}


		if (xQueueReceive(QueueButtons,&ButtonMsg,0) == pdTRUE)
		{
			if(ButtonMsg.ButtonStatus)							//irgend ein schalter ein
			{
				SPIMsg.flashtime=ButtonMsg.ButtonStatus;		//LED On-Zeit kann mit Schalter eingestellt werden
			}
			else												//Schalter0 aus
			{
				SPIMsg.flashtime=255-255*MotorMsg.PWMValue/100;//LED On-Zeit wird durch PWM-Wert bestimmt
			}
			LCDMsg.SwitchStatus=ButtonMsg.ButtonStatus;
			if(xQueueSend(QueueSPI,&SPIMsg,0)==pdTRUE)
			{

			}
			if(xQueueSend(QueueDisplay,&LCDMsg,0)==pdTRUE)
			{

			}

		}

		if(xQueueReceive(QueueUart,&UartMsg,0)==pdTRUE)
		{
			for(i=0;i<UART_MAXTEXTLENGTH;i++)
			{
				LCDMsg.MsgString[i]=buffer[i]=UartMsg.text[i];
				Cnt=0;
			}
			printf("\r\nFolgender Text wird auf Drehscheibe ausgegeben: %s",buffer);
			printf("\r\n Neuen Text eingeben (max %u Zeichen):", UART_MAXTEXTLENGTH);
			if(xQueueSend(QueueDisplay,&LCDMsg,0)==pdTRUE)
			{

			}
		}



/* Character Handling *******************************************************************/

		if(Cnt==0)
		{
			switch(buffer[Cnt])
			{
			case '!': localCharacterCounter = 26 * INT_PRO_CHAR + 1;break;
			case '?': localCharacterCounter = 27 * INT_PRO_CHAR + 1;break;
			case '-': localCharacterCounter = 28 * INT_PRO_CHAR + 1;break;
			case '.': localCharacterCounter = 29 * INT_PRO_CHAR + 1;break;
			case ',': localCharacterCounter = 30 * INT_PRO_CHAR + 1;break;
			case ':': localCharacterCounter = 31 * INT_PRO_CHAR + 1;break;
			case ';': localCharacterCounter = 32 * INT_PRO_CHAR + 1;break;
			case ' ': localCharacterCounter = 34 * INT_PRO_CHAR + 1;break;
			default: localCharacterCounter = (buffer[Cnt] - 65) * INT_PRO_CHAR+1;
			}
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
					switch(buffer[Cnt])
					{
					case '!': localCharacterCounter = 26 * INT_PRO_CHAR + 1;break;
					case '?': localCharacterCounter = 27 * INT_PRO_CHAR + 1;break;
					case '-': localCharacterCounter = 28 * INT_PRO_CHAR + 1;break;
					case '.': localCharacterCounter = 29 * INT_PRO_CHAR + 1;break;
					case ',': localCharacterCounter = 30 * INT_PRO_CHAR + 1;break;
					case ':': localCharacterCounter = 31 * INT_PRO_CHAR + 1;break;
					case ';': localCharacterCounter = 32 * INT_PRO_CHAR + 1;break;
					case ' ': localCharacterCounter = 34 * INT_PRO_CHAR + 1;break;
					default: localCharacterCounter = (buffer[Cnt] - 65) * INT_PRO_CHAR+1;
					}
					portDISABLE_INTERRUPTS();
					CharacterCounter = localCharacterCounter;
					portENABLE_INTERRUPTS();
					xLastWakeTime=xTaskGetTickCount();
					Cnt++;
				}
				else
				{
					localCharacterCounter = 34 * INT_PRO_CHAR + 1;
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
 * @}
 */
