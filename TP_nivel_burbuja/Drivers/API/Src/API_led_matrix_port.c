/*
 * led_matrix_port.c
 *
 *  Created on: Mar 25, 2025
 *      Author: jez
 */

#include <API_led_matrix_port.h>

void setPin(GPIO_TypeDef * port, uint16_t pin){
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
}

void clearPin(GPIO_TypeDef * port, uint16_t pin){
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
}

void sendCommand(SPI_HandleTypeDef * hspi, GPIO_TypeDef * csport, uint16_t cspin, uint8_t command, uint8_t value) {
	uint8_t buffer[2];
    buffer[0] = command;
    buffer[1] = value;

    clearPin(csport, cspin); // CS en bajo
    HAL_SPI_Transmit(hspi, buffer, 2, HAL_MAX_DELAY);
    setPin(csport, cspin);   // CS en alto
}
