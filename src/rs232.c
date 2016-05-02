/*
 * rs232.c
 *
 *  Created on: May 2, 2016
 *      Author: estudm
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
#include <rs232.h>

static QueueHandle_t *pvUartQueue;

 void uart_init(QueueHandle_t *pvQueue)
{
	pvUartQueue=pvQueue;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    USART_StructInit(&USART_InitStruct);
    USART_InitStruct.USART_BaudRate = 115200;
    CARME_UART_Init(CARME_UART0, &USART_InitStruct);
    USART_Cmd(CARME_UART0, DISABLE);
    USART_ITConfig(CARME_UART0, USART_IT_RXNE, ENABLE);
    USART_Cmd(CARME_UART0, ENABLE);
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}


void USART1_IRQHandler(void)
{
	static uint8_t len=0;				//Anzahl empfangene Zeichen
    uint8_t c;
    static Msg_Uart_t UartMsg;
    portBASE_TYPE xTaskWoken = pdFALSE ;
    while (USART_GetITStatus(CARME_UART0, USART_IT_RXNE) != RESET)
    {
		c=USART_ReceiveData(CARME_UART0);//Zeichen einlesen
		if(c==8)					 	//Wenn ein Backspace empfangen wurde
		{
			if(len>0)				 	//Wenn schon Zeichen empfangen wurden
				{
				len--;				 	//Zeichen dekrementieren
				USART_SendData(CARME_UART0, c);		 	//Backspace zurücksenden
				USART_SendData(CARME_UART0, ' ');	 	    //gelöschtes Zeichen überschreiben
				USART_SendData(CARME_UART0, c);		 	//Backspace zurücksenden
				}
		}
		else if (c==13)					//Carriage Return empfangen
		{
			UartMsg.text[len]=0;			//Nullterminierung
			len=0;						//Zähler zurücksetzen
			xQueueSendToBackFromISR(*pvUartQueue,&UartMsg,&xTaskWoken);
			portEND_SWITCHING_ISR(xTaskWoken);

		}
		else if ((c>=32)&&(c<=126))	 	//Wenn druckbares Zeichen empfangen wurde
		{
			if(len<(UART_MAXTEXTLENGTH-1)) //Wenn maximale Stringlänge noch nicht erreicht
			{
				UartMsg.text[len++]=c;	//Empfangenes Zeichen in String ablegen
				USART_SendData(CARME_UART0, c);			//Empfangenes Zeichen zurücksenden
			}
		}
    }
}
