/*
 * motor_pwm.h
 *
 *  Created on: Apr 18, 2016
 *      Author: estudm
 */

#ifndef MOTOR_PWM_H_
#define MOTOR_PWM_H_




void Motor_Init(uint8_t PWMvalue);
void Motor_SetDirection(uint8_t SetDirection);
void Motor_SetPWMValue(uint8_t PWMvalue);

#endif /* MOTOR_PWM_H_ */
