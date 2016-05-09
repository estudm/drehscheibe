/**
 *****************************************************************************
 * @defgroup	buttons
 * @{
 *
 * @file		buttons.c
 * @version		1.0
 * @date		02.05.2016
 * @author		kohll6, studm12
 *
 * @brief		Task der die acht Schalter auf dem CARME-IO1 Modul einliest.
 *
 *****************************************************************************
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

/* Task Settings */
#define PRIORITY_BUTTONTASK (3u)
#define STACKSIZE_BUTTONTASK (128)
#define QUEUE_SIZE_BUTTON (3)

/**
 * @brief Nachricht an Control-Task
 */
typedef struct
{
	uint8_t ButtonStatus; /**< Status der acht Schalter */
} Msg_Buttons_t;

void ButtonTask(void *pvargs);



#endif /* BUTTONS_H_ */
/**
 * @}
 */
