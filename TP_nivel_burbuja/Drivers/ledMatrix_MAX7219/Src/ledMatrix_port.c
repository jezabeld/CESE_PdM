/**
 * @file ledMatrix_port.c
 * @brief Device Driver para matriz de LEDs de 8x8 con controlador MAX7219. Funciones para el hardware específico.
 *
 * Creado como parte del Trabajo Práctico final de las materias `Protocolos de
 * 	Comunicación en Sistemas embebidos` y `Programación de Microcontroladores`.
 *
 * @author  Jez
 * @date Mar 25, 2025
 */

#include "assert.h"
#include "ledMatrix_port.h"

void setPin(GPIO_TypeDef * port, uint16_t pin){
	assert(port);
	assert_param(pin >= 0 && pin <16);

	HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
}

void clearPin(GPIO_TypeDef * port, uint16_t pin){
	assert(port);
	assert_param(pin >= 0 && pin <16);

	HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
}

bool sendCommand(SPI_HandleTypeDef * hspi, GPIO_TypeDef * csport, uint16_t cspin, uint8_t command, uint8_t value) {
	assert(hspi);
	assert(csport);
	assert_param(cspin >= 0 && cspin <16);

	HAL_StatusTypeDef status;
	uint8_t buffer[2];
    buffer[0] = command;
    buffer[1] = value;

    clearPin(csport, cspin); // CS en bajo
    status = HAL_SPI_Transmit(hspi, buffer, 2, HAL_MAX_DELAY);
    setPin(csport, cspin);   // CS en alto

    if(status != HAL_OK){
    	return true;
    }
    return false;
}
