/**
 *****************************************************************************
 * @defgroup	Motor
 * @{
 *
 * @file		motor_pwm.c
 * @version		1.0
 * @date		02.05.2016
 * @author		kohll6, studm12
 *
 * @brief		Motor-Task und Funktionen zur Steuerung des Motors.
 *
 *****************************************************************************
 */
#include <stdio.h>					/* Standard input and output			*/
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <FreeRTOS.h>				/* FreeRTOS								*/
#include <task.h>					/* FreeRTOS tasks						*/
#include <queue.h>					/* FreeRTOS queues						*/
#include <semphr.h>					/* FreeRTOS semaphores					*/
#include <carme.h>					/* CARME Module							*/
#include <carme_io2.h>
#include "motor_pwm.h"



#define PWM_HZ ( 650000 ) 			/* PWM counter speed */
#define PWM_PERIOD ( 100 ) 			/* PWM period length */

static QueueHandle_t *pvMotorQueue; /* Pointer auf Queue-Handle*/

static void Motor_Init(uint8_t PWMvalue);
static void Motor_SetDirection(CARME_IO2_PWM_PHASE dir);
static void Motor_SetPWMValue(uint8_t PWMvalue);


/**
 * @brief Task zur Steuerung des Motors
 * @param pvargs Pointer auf Handle der zu verwendenden MessageQueue
 */
void MotorTask (void *pvargs)
{
	if(pvargs!=NULL)	/*Parameter vorhanden*/
	{
		pvMotorQueue=(QueueHandle_t *) pvargs;	/*QueueHandle setzen*/
		Motor_Init(0);							/*PWM initialisieren*/
		for(;;)
		{
			Msg_Motor_t MotorMsg;				/*Lokale Message*/

			/*Message empfangen*/
			if(xQueueReceive(*pvMotorQueue,&MotorMsg,portMAX_DELAY)==pdTRUE)
			{
				Motor_SetDirection(MotorMsg.dir);		/*Neue Motoreinstellungen schreiben*/
				Motor_SetPWMValue(MotorMsg.PWMValue);

			}

		}
	}
}


/**
 * @brief Initialisiert das PWM-Modul und startet den Motor mit gewünschtem PWM-Wert
 * @param PWMvalue PWM-Impulsdauer in % [0..100]
 */
static void Motor_Init(uint8_t PWMvalue)
{
	uint16_t u16Prescaler;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	/* Calculate clock prescaler */
	u16Prescaler = (uint16_t) (((SystemCoreClock) / 2) / PWM_HZ) - 1;
	/* Initialize pwm clock structure */
	TIM_TimeBaseStruct.TIM_Prescaler = u16Prescaler;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStruct.TIM_Period = PWM_PERIOD - 1;
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
	CARME_IO2_PWM_Settings(&TIM_TimeBaseStruct);
	/* Set start output value of pwm to zero */
	CARME_IO2_PWM_Set(CARME_IO2_PWM0, 0);
	CARME_IO2_PWM_Set(CARME_IO2_PWM1, 0);
	CARME_IO2_PWM_Set(CARME_IO2_PWM2, 0);
	CARME_IO2_PWM_Set(CARME_IO2_PWM3, 0);
	/* Set direction of pwm3 to normal */
	CARME_IO2_PWM_Phase(CARME_IO2_PWM_NORMAL_DIRECTION);
	/* Redefine PWM0 to PWM2 as normal gpio's. Just pwm3 is defined as pwm */
	CARME_IO2_GPIO_OUT_Settings(CARME_IO2_GPIO_OUT_PIN0, CARME_IO2_GPIO_OUT_MODE_GPIO);
	CARME_IO2_GPIO_OUT_Settings(CARME_IO2_GPIO_OUT_PIN1, CARME_IO2_GPIO_OUT_MODE_GPIO);
	CARME_IO2_GPIO_OUT_Settings(CARME_IO2_GPIO_OUT_PIN2, CARME_IO2_GPIO_OUT_MODE_GPIO);

	/* Limit value to max period */
	if(PWMvalue > (PWM_PERIOD - 1)) {
		PWMvalue = PWM_PERIOD - 1;
	}
	CARME_IO2_PWM_Set(CARME_IO2_PWM3, PWMvalue);

}


/**
 * @brief Stellt Drehrichtung des Motors ein
 * @param dir Normale oder inverse Drehrichtuung
 */
static void Motor_SetDirection(CARME_IO2_PWM_PHASE dir)
{
	if(dir==CARME_IO2_PWM_NORMAL_DIRECTION)
	{
	/* Set direction of pwm3 to normal */
		CARME_IO2_PWM_Phase(CARME_IO2_PWM_NORMAL_DIRECTION);
	}
	else
	{
		CARME_IO2_PWM_Phase(CARME_IO2_PWM_OPPOSITE_DIRECTION);
	}
}

/**
 * @brief Ändert Drehgeschwindigkeit des Motors
 * @param PWMvalue Drehgeschwindigkeit in % [0..100]
 */
static void Motor_SetPWMValue(uint8_t PWMvalue)
{
	/* Limit value to max period */
		if(PWMvalue > (PWM_PERIOD - 1)) {
			PWMvalue = PWM_PERIOD - 1;
		}
		CARME_IO2_PWM_Set(CARME_IO2_PWM3, PWMvalue);
}


/**
 * @}
 */
