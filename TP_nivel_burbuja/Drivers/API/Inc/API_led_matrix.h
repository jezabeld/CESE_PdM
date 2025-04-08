/*
 * led_matrix.h
 *
 *  Created on: Mar 25, 2025
 *      Author: jez
 */

#ifndef API_INC_API_LED_MATRIX_H_
#define API_INC_API_LED_MATRIX_H_

#include <API_led_matrix_port.h>

#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8

/**
 * @typedef delay_t
 * @brief Estructura de los retardos.
 */
typedef struct{
   GPIO_TypeDef * csPort;
   uint16_t csPin;
   SPI_HandleTypeDef * hSpi;
} ledMatrix_t;

void ledMatrixInit(ledMatrix_t * ledMatrix, SPI_HandleTypeDef * hSpi, GPIO_TypeDef * csPort, uint16_t csPin);
void ledMatrixShutdown(ledMatrix_t * ledMatrix, uint8_t sd);
void ledMatrixClear(ledMatrix_t * ledMatrix);
void ledMatrixRender(ledMatrix_t * ledMatrix, uint8_t * screen);

#endif /* API_INC_API_LED_MATRIX_H_ */
