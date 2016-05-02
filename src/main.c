/**
 *****************************************************************************
 * @defgroup	Control_Main
 * @{
 *
 * @file		main.c
 * @version		1.0
 * @date		02.05.2016
 * @author		kohll6, studm12
 *
 * @brief		Main-Source File with implemented Control Task
 *
 *****************************************************************************
 */

/*----- Header-Files -------------------------------------------------------*/
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
#include "gpio_ISR.h"
#include "motor_pwm.h"
#include "spi.h"
#include "poti.h"
#include "buttons.h"
#include "rs232.h"
#include "display.h"
/*----- Macros -------------------------------------------------------------*/
#define INT_PRO_CHAR	(14)		/*Anzahl Interrupts pro Zeichen (500 Int/U)*/#define PRIORITY_CONTROLTASK (4)
#define STACKSIZE_CONTROLTASK (128)
/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/
static void ControlTask(void *pvargs);

/*----- Data ---------------------------------------------------------------*/
QueueHandle_t QueueButtons;
QueueHandle_t QueueMotor;
QueueHandle_t QueuePoti;
QueueHandle_t QueueSPI;
QueueHandle_t QueueUart;
QueueHandle_t QueueDisplay;

uint8_t LedGPIO = 0;

/*----- Implementation -----------------------------------------------------*/
/**
 * @brief		main
 * @return		0 if success
 */
int main(void) {

	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);


	/* Create Queues */
	QueueButtons = xQueueCreate(QUEUE_SIZE_BUTTON, sizeof(Msg_Buttons_t));
	QueuePoti=xQueueCreate(QUEUE_SIZE_POTI,sizeof(Msg_Poti_t));
	QueueMotor=xQueueCreate(QUEUE_SIZE_MOTORTASK,sizeof(Msg_Motor_t));
	QueueSPI=xQueueCreate(QUEUE_SIZE_SPI,sizeof(Msg_SPI_t));
	QueueUart=xQueueCreate(QUEUE_SIZE_UART,sizeof(Msg_Uart_t));
	QueueDisplay=xQueueCreate(QUEUE_SIZE_DISPLAY,sizeof (Msg_Display_t));

	/*Init Hardware*/
	CARME_IO1_Init();
	CARME_IO2_Init();
	uart_init(&QueueUart);		/*Init UART-Hardware & Interrupt*/
	gpio_init();				/*Init GPIO-Hardware & Interrupt*/

	/*Create Tasks*/
	xTaskCreate(ControlTask, "Control", STACKSIZE_CONTROLTASK, NULL,PRIORITY_CONTROLTASK, NULL);
	xTaskCreate(PotiTask, "PotiTask", STACKSIZE_POTITASK,(void *) &QueuePoti,	PRIORITY_POTITASK, NULL);
	xTaskCreate(ButtonTask, "ButtonTask", STACKSIZE_BUTTONTASK, (void *)&QueueButtons,PRIORITY_BUTTONTASK, NULL);
	xTaskCreate(MotorTask, "Motor Task", STACKSIZE_MOTORTASK, (void *)&QueueMotor,PRIORITY_MOTORTASK, NULL);
	xTaskCreate(SPITask,"SPI Task",STACKSIZE_SPITASK,(void*) &QueueSPI,PRIORITY_SPITASK,NULL);
	xTaskCreate(DisplayTask, "Display Task",STACKSIZE_DISPLAYTASK, (void*) &QueueDisplay,PRIORITY_DISPLAYTASK,NULL);

	vTaskStartScheduler();		/* Start Scheduler*/
	for (;;) {

	}
	return 0;
}

/**
 * @brief 		Haupt-Task.
 *
 * @param		pvargs not used
 */
static void ControlTask(void *pvargs)
{
	portTickType xLastWakeTime = xTaskGetTickCount();	/*Letzter Aufruf*/
	char buffer[UART_MAXTEXTLENGTH]={0};				/*Buffer für Ausgabe über Modell*/
	uint32_t localCharacterCounter;						/*Zähler für Anzahl Interrupts die erreicht werden müssen für bestimmten Buchstaben*/
	uint8_t Cnt = 0;									/*Buchstabenzähler*/
	uint32_t i=0;										/*Schleifenzähler*/

	/* Lokale Messages*/
	Msg_Buttons_t 	ButtonMsg;			/*Nachricht von Button-Task*/
	Msg_Poti_t		PotiMsg;			/*Nachricht von Poti-Task*/
	Msg_Motor_t		MotorMsg;			/*Nachricht an Motor-Task*/
	Msg_SPI_t		SPIMsg;				/*Nachricht an SPI-Task*/
	Msg_Uart_t		UartMsg;			/*Nachricht von UART-Interrupt*/
	Msg_Display_t	DisplayMsg;			/*Nachricht an Display*/



	/*Initialausgabe UART*/
	snprintf(buffer, sizeof(buffer), "HALLO");
	snprintf(DisplayMsg.MsgString,sizeof(DisplayMsg.MsgString),"HALLO");
	printf("\r\n Neuen Text eingeben (max %u Zeichen):", UART_MAXTEXTLENGTH);


	for (;;)
	{
		/*Poti-Wert empfangen (alle 100ms)*/
		if(xQueueReceive(QueuePoti,&PotiMsg,0)==pdTRUE)
		{
			MotorMsg.PWMValue=PotiMsg.PotiVal;					/*Message an Motor-Task*/
			MotorMsg.dir=CARME_IO2_PWM_NORMAL_DIRECTION;

			DisplayMsg.PotiStatus=PotiMsg.PotiVal;				/*Message an Display-Task*/

			if(xQueueSend(QueueMotor,&MotorMsg,0)==pdTRUE)		/*Message an MotorTask senden*/
			{

			}
			if(xQueueSend(QueueDisplay,&DisplayMsg,0)==pdTRUE)	/*Message an Display-Task senden*/
			{

			}
		}


		/*Button-Werte empfangen (alle 100ms)*/
		if (xQueueReceive(QueueButtons,&ButtonMsg,0) == pdTRUE)
		{
			if(ButtonMsg.ButtonStatus)							/*mindestens ein Schalter ein*/
			{
				SPIMsg.flashtime=ButtonMsg.ButtonStatus;		/*LED On-Zeit wird über Schalter-Wert bestimmt*/
			}
			else												/*Alle Schalter aus*/
			{
				SPIMsg.flashtime=255-255*MotorMsg.PWMValue/100;	/*LED On-Zeit wird durch PWM-Wert bestimmt*/
			}

			DisplayMsg.SwitchStatus=ButtonMsg.ButtonStatus;		/*Message an Display-Task*/

			if(xQueueSend(QueueSPI,&SPIMsg,0)==pdTRUE)			/*LED-On-Zeiten an SPI-Task senden*/
			{

			}
			if(xQueueSend(QueueDisplay,&DisplayMsg,0)==pdTRUE)	/*Message an Display-Task senden*/
			{

			}

		}


		/*UART-String empfangen*/
		if(xQueueReceive(QueueUart,&UartMsg,0)==pdTRUE)
		{
			for(i=0;i<UART_MAXTEXTLENGTH;i++)					/*String in Buffer und LCD-Message schreiben*/
			{
				DisplayMsg.MsgString[i]=buffer[i]=UartMsg.text[i];
				Cnt=0;
			}
			/*Ausgabe UART*/
			printf("\r\nFolgender Text wird auf Drehscheibe ausgegeben: %s",buffer);
			printf("\r\n Neuen Text eingeben (max %u Zeichen):", UART_MAXTEXTLENGTH);

			/*Message an LCD-Task senden*/
			if(xQueueSend(QueueDisplay,&DisplayMsg,0)==pdTRUE)
			{

			}
		}



/* Character Handling *******************************************************************/

		if(Cnt==0)				/*Erster Buchstabe*/
		{
			switch(buffer[Cnt])	/*Umwandeln des Charakters in die benötigte Anzahl Interrupts*/
			{
			case '!': localCharacterCounter = 26 * INT_PRO_CHAR + 1;break;
			case '?': localCharacterCounter = 27 * INT_PRO_CHAR + 1;break;
			case '-': localCharacterCounter = 28 * INT_PRO_CHAR + 1;break;
			case '.': localCharacterCounter = 29 * INT_PRO_CHAR + 1;break;
			case ',': localCharacterCounter = 30 * INT_PRO_CHAR + 1;break;
			case ':': localCharacterCounter = 31 * INT_PRO_CHAR + 1;break;
			case ';': localCharacterCounter = 32 * INT_PRO_CHAR + 1;break;
			case ' ': localCharacterCounter = 34 * INT_PRO_CHAR + 1;break;
			default: localCharacterCounter = (buffer[Cnt] - 65) * INT_PRO_CHAR+1;
			}

			portDISABLE_INTERRUPTS();						/*Interrupts deaktivieren*/
			ISRCharacterCounter = localCharacterCounter;	/*Neuen Zähler für ISR schreiben*/
			portENABLE_INTERRUPTS();						/*Interrupts aktivieren*/
			xLastWakeTime=xTaskGetTickCount();				/*Tick-Zähler erneuern*/
			Cnt++;
		}
		else		/*nicht erster Buchstabe*/
		{
			if(xTaskGetTickCount()-xLastWakeTime>1000)	/*nach einer Sekunde*/
			{

				if (buffer[Cnt] != 0)		/*Buchstabe vorhanden, keine Nullterminierung*/
				{
					switch(buffer[Cnt])		/*Umwandeln des Charakters in die benötigte Anzahl Interrupts*/
					{
					case '!': localCharacterCounter = 26 * INT_PRO_CHAR + 1;break;	/*27. Zeichen auf Scheibe*/
					case '?': localCharacterCounter = 27 * INT_PRO_CHAR + 1;break;	/*28. Zeichen auf Scheibe*/
					case '-': localCharacterCounter = 28 * INT_PRO_CHAR + 1;break;	/*...*/
					case '.': localCharacterCounter = 29 * INT_PRO_CHAR + 1;break;
					case ',': localCharacterCounter = 30 * INT_PRO_CHAR + 1;break;
					case ':': localCharacterCounter = 31 * INT_PRO_CHAR + 1;break;
					case ';': localCharacterCounter = 32 * INT_PRO_CHAR + 1;break;
					case ' ': localCharacterCounter = 34 * INT_PRO_CHAR + 1;break;
					default: localCharacterCounter = (buffer[Cnt] - 65) * INT_PRO_CHAR+1;
					}
					portDISABLE_INTERRUPTS();					/*Interrupts deaktivieren*/
					ISRCharacterCounter = localCharacterCounter; 	/*Neuen Zähler für ISR schreiben*/
					portENABLE_INTERRUPTS();					/*Interrupts aktivieren*/
					xLastWakeTime=xTaskGetTickCount();			/*Tick-Zähler erneuern*/
					Cnt++;
				}
				else						/*Nullterminierung --> Text zu Ende*/
				{
					localCharacterCounter = 34 * INT_PRO_CHAR + 1;	/*Ausgabe leerer Charakter*/
					portDISABLE_INTERRUPTS();
					ISRCharacterCounter = localCharacterCounter;	/*Neuen Zähler für ISR schreiben*/
					portENABLE_INTERRUPTS();
					if(xTaskGetTickCount()-xLastWakeTime>2000)		/*Nach zwei Sekunden Leerzeichen-Ausgabe*/
					{
						Cnt = 0;									/*Von vorne beginnen*/
					}

				}
			}
		}
		vTaskDelay(10);
	}
}


/**
 * @}
 */
