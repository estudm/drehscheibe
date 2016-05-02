/*
 * lcd.c
 *
 *  Created on: May 2, 2016
 *      Author: estudm
 */
#include <stdio.h>					/* Standard input and output			*/
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/

#include <FreeRTOS.h>				/* FreeRTOS								*/
#include <task.h>					/* FreeRTOS tasks						*/
#include <queue.h>					/* FreeRTOS queues						*/
#include <semphr.h>					/* FreeRTOS semaphores					*/
#include <memPoolService.h>			/* Memory pool manager service			*/

#include <carme_io1.h>
#include <carme_io2.h>
#include "lcd.h"
#include <display.h>


static QueueHandle_t *pvLCDQueue;

void LCDTask(void *pvargs) {
	Msg_LCD_t LCDMsg;
	char PotiString[32];
	char SwitchString[32];
	char WordString[32];

	LCD_Init();
	if (pvargs != NULL) {
		pvLCDQueue = (QueueHandle_t *) pvargs;

		for (;;) {
			if (xQueueReceive(*pvLCDQueue,&LCDMsg, portMAX_DELAY) == pdTRUE) {
				snprintf(PotiString, 32, "Potentiometer: %d",
						LCDMsg.PotiStatus);
				snprintf(SwitchString, 32, "Button: %d", LCDMsg.SwitchStatus);
				snprintf(WordString, 32, "Ausgabe: %s", LCDMsg.MsgString);
			}
			LCD_ClearLine(0);
			LCD_ClearLine(1);
			LCD_ClearLine(2);
			LCD_DisplayStringLine(0, SwitchString);
			LCD_DisplayStringLine(1, PotiString);
			LCD_DisplayStringLine(2, WordString);
			vTaskDelay(100);
		}
	}
}

