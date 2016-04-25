/*
 * spi.h
 *
 *  Created on: Apr 25, 2016
 *      Author: Lukas
 */

#ifndef SPI_H_
#define SPI_H_

void SPITask(void *pvargs);

#define PRIORITY_SPITASK (4u)
#define STACKSIZE_SPITASK (128)
#define QUEUE_SIZE_SPI (3)

typedef struct {
uint8_t flashtime;} Msg_SPI_t;

#endif /* SPI_H_ */
