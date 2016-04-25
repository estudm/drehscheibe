/*
 * poti.h
 *
 *  Created on: Apr 18, 2016
 *      Author: estudm
 */

#ifndef POTI_H_
#define POTI_H_

typedef struct
{
	uint8_t PotiVal;
}Msg_Poti_t;

void PotiTask(void *pvargs);

#define PRIORITY_POTITASK (3u)
#define STACKSIZE_POTITASK (128)
#define QUEUE_SIZE_POTI (3)
#endif /* POTI_H_ */
