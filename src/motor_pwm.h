/*
 * motor_pwm.h
 *
 *  Created on: Apr 18, 2016
 *      Author: estudm
 */

#ifndef MOTOR_PWM_H_
#define MOTOR_PWM_H_

/**
 * @brief Nachricht zur Steuerung des Motors
 */
typedef struct
{
	uint8_t PWMValue;		/**< Geschwindigkeit */
	CARME_IO2_PWM_PHASE dir;/**< Drehrichtung */
}Msg_Motor_t;


/* Task-Einstellungen */
#define STACKSIZE_MOTORTASK (128)
#define PRIORITY_MOTORTASK (3)
#define QUEUE_SIZE_MOTORTASK (3)



void MotorTask(void *pvargs);

#endif /* MOTOR_PWM_H_ */
