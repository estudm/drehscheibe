/**
 *****************************************************************************
 * @defgroup	GPIO-Interrupts
 * @{
 *
 * @file		gpio.h
 * @version		1.0
 * @date		02.05.2016
 * @author		kohll6, studm12
 *
 * @brief		Interruptroutine für Index und Kanal A
 *
 *****************************************************************************
 */
#ifndef GPIO_ISR_H_
#define GPIO_ISR_H_

/* Funktions-Prototypen */
void gpio_init(void) ;
void MyEXTI9_5_IRQHandler(void);

/* Exportierte Daten*/
extern  uint32_t ISRCharacterCounter;

#endif /* GPIO_ISR_H_ */
/**
 * @}
 */
