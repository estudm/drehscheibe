/*
 * poti.h
 *
 *  Created on: Apr 18, 2016
 *      Author: estudm
 */

#ifndef POTI_H_
#define POTI_H_

/**
 * @brief Nachricht von Poti-Task
 */
typedef struct
{
	uint8_t PotiVal; /**< Aktuell eingestellter Poti-Wert*/
}Msg_Poti_t;

/* Task-Einstellungen */
#define PRIORITY_POTITASK (3u)
#define STACKSIZE_POTITASK (128)
#define QUEUE_SIZE_POTI (3)

/*Funktionsprototyp*/
void PotiTask(void *pvargs);


#endif /* POTI_H_ */
