/**
 *****************************************************************************
 * @defgroup	SPI
 * @{
 *
 * @file		spi.c
 * @version		1.0
 * @date		02.05.2016
 * @author		kohll6, studm12
 *
 * @brief		SPI-Initialisierung und SPI-Task
 *
 *****************************************************************************
 */

#ifndef SPI_H_
#define SPI_H_

void SPITask(void *pvargs);

/* Task Settings*/
#define PRIORITY_SPITASK (4u)
#define STACKSIZE_SPITASK (128)
#define QUEUE_SIZE_SPI (3)


/**
 * @brief Nachricht an SPI-Task
 */
typedef struct
{
	uint8_t flashtime; /**< Blitzdauer der LED */
} Msg_SPI_t;

#endif /* SPI_H_ */
/**
 * @}
 */
