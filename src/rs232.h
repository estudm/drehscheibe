/*
 * rs232.h
 *
 *  Created on: May 2, 2016
 *      Author: estudm
 */

#ifndef RS232_H_
#define RS232_H_

#define UART_MAXTEXTLENGTH 	(32)/**< Maximale Länge eines Strings der empfangen werden darf*/
#define QUEUE_SIZE_UART 	(3)	/**< Grösse der Queue*/


/**
 * @brief Nachricht von UART-RX-Interrupt
 */
typedef struct
{
	uint8_t text[UART_MAXTEXTLENGTH];/**< Empfangener Text*/

}Msg_Uart_t;

void MyUSART1_IRQHandler(void);
void uart_init(QueueHandle_t *pvQueue);




#endif /* RS232_H_ */
