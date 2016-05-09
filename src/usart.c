/**
 *****************************************************************************
 * @defgroup	UART-Interrupt
 * @{
 *
 * @file		usart.c
 * @version		1.0
 * @date		02.05.2016
 * @author		kohll6, studm12
 *
 * @brief		UART Receive-interrupt
 *
 *****************************************************************************
 */
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
#include <uart.h>
#include <usart.h>
#include <ctype.h>

/* POinter auf QueueHandle*/
static QueueHandle_t *pvUartQueue;

/**
 * @brief Initialisiert die UART und den Empfangs-Interrupt.
 * @param pvQueue Pointer auf QueueHandle
 */
void uart_init(QueueHandle_t *pvQueue)
{
	pvUartQueue=pvQueue;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    /* Init UART*/
    USART_StructInit(&USART_InitStruct);
    USART_InitStruct.USART_BaudRate = 115200;
    CARME_UART_Init(CARME_UART0, &USART_InitStruct);
    USART_Cmd(CARME_UART0, DISABLE);
    USART_ITConfig(CARME_UART0, USART_IT_RXNE, ENABLE);
    USART_Cmd(CARME_UART0, ENABLE);
    /*Init NVIC*/
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}


/**
 * @brief UART-RX Interrupt
 */
void MyUSART1_IRQHandler(void)
{
	static uint8_t len=0;											/*Anzahl empfangene Zeichen*/
    uint8_t c;														/*Empfangener Charakter*/
    static Msg_Uart_t UartMsg;										/*lokale Nachricht*/

    portBASE_TYPE xTaskWoken = pdFALSE ;
    while (USART_GetITStatus(CARME_UART0, USART_IT_RXNE) != RESET)	/*solange Zeichen verfügbar*/
    {
		c=USART_ReceiveData(CARME_UART0);							/*Zeichen einlesen*/
		if(c==8)					 								/*Wenn ein Backspace empfangen wurde*/
		{
			if(len>0)				 								/*Wenn schon Zeichen empfangen wurden*/
				{
				len--;				 								/*Zeichen dekrementieren*/
				USART_SendData(CARME_UART0, c);		 				/*Backspace zurücksenden*/
				USART_SendData(CARME_UART0, ' ');	 	   			/*gelöschtes Zeichen überschreiben*/
				USART_SendData(CARME_UART0, c);		 				/*Backspace zurücksenden*/
				}
		}
		else if (c==13)												/*Carriage Return empfangen*/
		{
			UartMsg.text[len]=0;									/*Nullterminierung*/
			len=0;													/*Zähler zurücksetzen*/
			xQueueSendToBackFromISR(*pvUartQueue,&UartMsg,&xTaskWoken);/*Nachricht senden*/


		}
		else if ((c>=65)&&(c<=90))	 								/*Wenn Grossbuchstabe*/
		{
			if(len<(UART_MAXTEXTLENGTH-1)) /*Wenn maximale Stringlänge noch nicht erreicht*/
			{
				UartMsg.text[len++]=c;	/*Empfangenes Zeichen in String ablegen*/
				USART_SendData(CARME_UART0, c);			/*Empfangenes Zeichen zurücksenden*/
			}
		}
		else if ((c>=97)&&(c<=122))	 	/*Wenn Kleinbuchstabe*/
		{
			if(len<(UART_MAXTEXTLENGTH-1)) /*Wenn maximale Stringlänge noch nicht erreicht*/
			{
				c=toupper(c);				/*Zu Grossbuchstdabe konvertieren*/
				UartMsg.text[len++]=c;	/*Empfangenes Zeichen in String ablegen*/
				USART_SendData(CARME_UART0, c);			/*Empfangenes Zeichen zurücksenden*/
			}
		}
		/*falls verfügbares Sonderzeichen*/
		else if((c=='!')||(c=='?')||(c=='-')||(c=='.')||(c==',')||(c==':')||(c==';')||(c==' '))
		{
			if(len<(UART_MAXTEXTLENGTH-1)) /*Wenn maximale Stringlänge noch nicht erreicht*/
				{
					UartMsg.text[len++]=c;	/*Empfangenes Zeichen in String ablegen*/
					USART_SendData(CARME_UART0, c);			/*Empfangenes Zeichen zurücksenden*/
				}
		}
    }
   /* portEND_SWITCHING_ISR(xTaskWoken);*/
    portYIELD_FROM_ISR(xTaskWoken);
}

/**
 * @}
 */
