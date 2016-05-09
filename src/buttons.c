/**
 *****************************************************************************
 * @defgroup	buttons
 * @{
 *
 * @file		buttons.c
 * @version		1.0
 * @date		02.05.2016
 * @author		kohll6, studm12
 *
 * @brief		Task der die acht Schalter auf dem CARME-IO1 Modul einliest.
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
#include <memPoolService.h>			/* Memory pool manager service			*/

#include <carme_io1.h>
#include <carme_io2.h>
#include <buttons.h>


/* Pointer auf Queuehandle */
static QueueHandle_t *pvButtonQueue;



void ButtonTask(void *pvargs)
{
	uint8_t ButtonValue=0;		/* Eingelesener Wert */
	Msg_Buttons_t ButtonMsg;	/* Nachricht an Control-Task */
	if(pvargs!=NULL)
	{
		pvButtonQueue=(QueueHandle_t *)pvargs;

		for (;;)
		{
			CARME_IO1_SWITCH_Get(&ButtonValue);					/* Buttons einlesen */
			ButtonMsg.ButtonStatus=ButtonValue;
			xQueueSendToBack(*pvButtonQueue, &ButtonMsg, 1); 	/* Nachricht an Control-Task senden */
			vTaskDelay(100);									/* alle 100ms wiederholen */
		}
	}
}
/**
 * @}
 */
