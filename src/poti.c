/**
 *****************************************************************************
 * @defgroup	Poti
 * @{
 *
 * @file		poti.c
 * @version		1.0
 * @date		02.05.2016
 * @author		kohll6, studm12
 *
 * @brief		Poti-Task
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
#include <poti.h>
#include <motor_pwm.h>

/*Pointer auf Queue-Handle*/
static QueueHandle_t * pvPotiQueue;

/**
 * @brief Task der  Poti ausliest
 * @param pvargs Pointer auf Queue-Handle
 */
void PotiTask(void *pvargs)
{
	if(pvargs!=NULL)		/*Parameter übergeben*/
	{
		pvPotiQueue=(QueueHandle_t *) pvargs;
		for(;;)
		{
			uint16_t ADCValue;
			Msg_Poti_t PotiMessage;
			CARME_IO2_ADC_Get(CARME_IO2_ADC_PORT0,&ADCValue);	/*Poti auslesen*/
			PotiMessage.PotiVal=(100*ADCValue/1024);			/*Umwandeln in Wert 0..100*/
			xQueueSendToBack(*pvPotiQueue,&PotiMessage,1);		/*Nachricht senden*/
			vTaskDelay(100);									/*alle 100ms wiederholen*/
		}
	}
}

/**
 * @}
 */

