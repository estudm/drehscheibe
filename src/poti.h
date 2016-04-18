/*
 * poti.h
 *
 *  Created on: Apr 18, 2016
 *      Author: estudm
 */

#ifndef POTI_H_
#define POTI_H_

void Poti_Init();
void PotiTask(void *pvargs);

#define PRIORITY_POTITASK (4u)
#define STACKSIZE_POTITASK (128)
#endif /* POTI_H_ */
