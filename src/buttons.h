/*
 * buttons.h
 *
 *  Created on: Apr 25, 2016
 *      Author: Lukas
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

void Buttons_Init();
void ButtonTask(void *pvargs);

#define PRIORITY_BUTTONTASK (5u)
#define STACKSIZE_BUTTONTASK (128)
#define QUEUE_SIZE_BUTTON (3)

typedef struct {
uint8_t ButtonStatus;} Msg_Buttons_t;

#endif /* BUTTONS_H_ */
