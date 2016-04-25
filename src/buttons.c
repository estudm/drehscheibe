/*
 * buttons.c
 *
 *  Created on: Apr 25, 2016
 *      Author: Lukas
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
#include <buttons.h>

QueueHandle_t *pvButtonQueue;

void Buttons_Init() {

}

void ButtonTask(void *pvargs) {
	uint8_t ButtonValue=0;
	if(pvargs!=NULL){
		pvButtonQueue=(QueueHandle_t *)pvargs;
	}
	for (;;) {
		CARME_IO1_SWITCH_Get(&ButtonValue);
		xQueueSend(ButtonQueue, &ButtonValue, portMAX_DELAY);
	}
}
