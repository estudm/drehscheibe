/*
 * motor_pwm.c
 *
 *  Created on: Apr 18, 2016
 *      Author: estudm
 */
#include <stdio.h>					/* Standard input and output			*/
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/
#include <carme_io2.h>
#include "motor_pwm.h"



#define PWM_HZ ( 650000 ) /* PWM counter speed */
#define PWM_PERIOD ( 100 ) /* PWM period length */

void MotorInit(uint8_t PWMvalue)
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

void Motor_SetDirection(CARME_IO2_PWM_PHASE dir)
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
void Motor_SetPWMValue(uint8_t PWMvalue)
{
	/* Limit value to max period */
		if(PWMvalue > (PWM_PERIOD - 1)) {
			PWMvalue = PWM_PERIOD - 1;
		}
		CARME_IO2_PWM_Set(CARME_IO2_PWM3, PWMvalue);
}

