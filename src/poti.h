/**
 *****************************************************************************
 * @defgroup	Poti
 * @{
 *
 * @file		poti.h
 * @version		1.0
 * @date		02.05.2016
 * @author		kohll6, studm12
 *
 * @brief		Poti-Task
 *
 *****************************************************************************
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
/**
 * @}
 */
