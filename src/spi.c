/*
 * spi.c
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
#include <spi.h>

#include <carme_io1.h>
#include <carme_io2.h>
#include <buttons.h>

QueueHandle_t *pvSPIQueue;

void SPIInit() {
	CARME_IO2_Init();
	//SPI Channel waehlen
	CARME_IO2_SPI_Select(CARME_IO2_nPSC1);
}
void SPITask(void *pvargs)
{
	Msg_SPI_t SPIMsg;
	SPIInit();
	if (pvargs != NULL)
	{
		pvSPIQueue = (QueueHandle_t *) pvargs;

		for (;;)
		{
			if (xQueueReceive(*pvSPIQueue,&SPIMsg,portMAX_DELAY) == pdTRUE)
			{
				CARME_IO2_SPI_CS_Out(0);
				CARME_IO2_SPI_Send(SPIMsg.flashtime);
				CARME_IO2_SPI_CS_Out(1);
			}
		}
	}

}
