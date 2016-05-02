/*
 * lcd.h
 *
 *  Created on: May 2, 2016
 *      Author: estudm
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#define PRIORITY_DISPLAYTASK (3u)
#define STACKSIZE_DISPLAYTASK (512)
#define QUEUE_SIZE_DISPLAY (3)

typedef struct
{
	uint8_t SwitchStatus;
	uint16_t PotiStatus;
	char MsgString[32];
} Msg_Display_t;

void DisplayTask(void *pvargs);

#endif /* LCD_H_ */
