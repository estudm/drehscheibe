/**
 *****************************************************************************
 * @defgroup	SPI
 * @{
 *
 * @file		spi.c
 * @version		1.0
 * @date		02.05.2016
 * @author		kohll6, studm12
 *
 * @brief		SPI-Initialisierung und SPI-Task
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
#include <spi.h>

#include <carme_io1.h>
#include <carme_io2.h>
#include <buttons.h>

/* Pointer auf Queuehandle*/
QueueHandle_t *pvSPIQueue;

/**
 * @brief Initialisierung der SPI-Schnittstelle.
 */
void SPIInit() {
	CARME_IO2_Init();
	//SPI Channel waehlen
	CARME_IO2_SPI_Select(CARME_IO2_nPSC1);
}

/**
 * @brief Aktualisiert LED-Blitzzeit via SPI.
 * Empfängt Nachrichten und sendet neue Blitzzeit als Binärwert via SPI an CPLD auf RT-Modell.
 * @param pvargs Pointer auf Queuehandle
 */
void SPITask(void *pvargs)
{
	Msg_SPI_t SPIMsg;
	SPIInit();
	if (pvargs != NULL)	/*Nur ausführen wenn Queuehandle übergeben bei Initialisierung*/
	{
		pvSPIQueue = (QueueHandle_t *) pvargs;

		for (;;)
		{
			if (xQueueReceive(*pvSPIQueue,&SPIMsg,portMAX_DELAY) == pdTRUE)	/*Warten auf Nachricht*/
			{
				CARME_IO2_SPI_CS_Out(0);
				CARME_IO2_SPI_Send(SPIMsg.flashtime);						/*Neue Blitzdauer senden*/
				CARME_IO2_SPI_CS_Out(1);
			}
		}
	}

}

/**
 * @}
 */
