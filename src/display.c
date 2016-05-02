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
	char SwitchString[64];
	char WordString[32];

	LCD_Init();
	LCD_SetFont(&font_8x16B);
	if (pvargs != NULL) {
		pvLCDQueue = (QueueHandle_t *) pvargs;

		for (;;)
		{
			if (xQueueReceive(*pvLCDQueue,&LCDMsg, portMAX_DELAY) == pdTRUE)
			{
				float Beleuchtung;
				snprintf(PotiString, 32, "Speed: %u%c",LCDMsg.PotiStatus,'%');
				if(LCDMsg.SwitchStatus==0)
				{
					Beleuchtung=(255-255*LCDMsg.PotiStatus/100)*0.25+0.25;
					snprintf(SwitchString, sizeof(SwitchString), "Blitzdauer: %02u us (Direkt /  Poti)",(uint32_t) Beleuchtung);
				}
				else
				{
					Beleuchtung=(0.25*LCDMsg.SwitchStatus)+0.25;
					snprintf(SwitchString, sizeof(SwitchString), "Blitzdauer: %02u us (Direkt / Schalter)",(uint32_t) Beleuchtung);
				}
				snprintf(WordString, 32, "Ausgabe: %s", LCDMsg.MsgString);

			LCD_ClearLine(0);
			LCD_DisplayStringLine(0, SwitchString);
			LCD_ClearLine(1);
			LCD_DisplayStringLine(1, PotiString);
			LCD_ClearLine(2);
			LCD_DisplayStringLine(2, WordString);
			}
		}
	}
}

