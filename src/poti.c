/*
 * poti.c
 *
 *  Created on: Apr 18, 2016
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
#include <poti.h>
#include <motor_pwm.h>

static QueueHandle_t * pvPotiQueue;
void PotiTask(void *pvargs)
{
	if(pvargs!=NULL)
	{
		pvPotiQueue=(QueueHandle_t *) pvargs;
		for(;;)
		{
			uint16_t ADCValue;
			Msg_Poti_t PotiMessage;
			CARME_IO2_ADC_Get(CARME_IO2_ADC_PORT0,&ADCValue);
			PotiMessage.PotiVal=(100*ADCValue/1024);
			xQueueSendToBack(*pvPotiQueue,&PotiMessage,1);
			vTaskDelay(100);
		}
	}
}
