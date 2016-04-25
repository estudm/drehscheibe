/*
 * buttons.h
 *
 *  Created on: Apr 25, 2016
 *      Author: Lukas
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#define PRIORITY_BUTTONTASK (3u)
#define STACKSIZE_BUTTONTASK (128)
#define QUEUE_SIZE_BUTTON (3)

typedef struct
{
	uint8_t ButtonStatus;
} Msg_Buttons_t;

void ButtonTask(void *pvargs);



#endif /* BUTTONS_H_ */
