/**
 *****************************************************************************
 * @defgroup	Display-Task
 * @{
 *
 * @file		display.h
 * @version		1.0
 * @date		02.05.2016
 * @author		kohll6, studm12
 *
 * @brief		Display-Task
 *
 *****************************************************************************
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

/* Task Settings */
#define PRIORITY_DISPLAYTASK (3u)
#define STACKSIZE_DISPLAYTASK (512)
#define QUEUE_SIZE_DISPLAY (3)

/**
 * @brief Nachricht an Display-Task mit den Einstellungen.
 */
typedef struct
{
	uint8_t SwitchStatus;	/**< Status der Schalter */
	uint16_t PotiStatus;	/**< Potistatus */
	char MsgString[32];		/**< Text der aktuell ausgegeben wird */
} Msg_Display_t;

void DisplayTask(void *pvargs);

#endif /* LCD_H_ */
/**
 * @}
 */
