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
void Poti_Init()
{

}
void PotiTask(void *pvargs)
{
for(;;)
{
	uint16_t ADCValue;
	CARME_IO2_ADC_Get(CARME_IO2_ADC_PORT0,&ADCValue);
	Motor_SetPWMValue(ADCValue%100);
	vTaskDelay(100);
}
}
