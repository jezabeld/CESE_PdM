/*
 * led_matrix_port.h
 *
 *  Created on: Mar 25, 2025
 *      Author: jez
 */

#ifndef API_INC_API_LED_MATRIX_PORT_H_
#define API_INC_API_LED_MATRIX_PORT_H_

#include "stdint.h"
#include "stm32f4xx_hal.h"

void setPin(GPIO_TypeDef * port, uint16_t pin);
void clearPin(GPIO_TypeDef * port, uint16_t pin);
void sendCommand(SPI_HandleTypeDef * hspi, GPIO_TypeDef * port, uint16_t pin, uint8_t command, uint8_t value);

#endif /* API_INC_API_LED_MATRIX_PORT_H_ */
