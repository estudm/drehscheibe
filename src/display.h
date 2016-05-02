/*
 * lcd.h
 *
 *  Created on: May 2, 2016
 *      Author: estudm
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#define PRIORITY_LCDTASK (3u)
#define STACKSIZE_LCDTASK (128)
#define QUEUE_SIZE_LCD (3)

typedef struct
{
	uint8_t SwitchStatus;
	uint16_t PotiStatus;
	char MsgString[32];
} Msg_LCD_t;

void LCDTask(void *pvargs);

#endif /* LCD_H_ */
