/*
 * led_matrix.c
 *
 *  Created on: Mar 25, 2025
 *      Author: jez
 */

#include "API_led_matrix.h"

// Driver registers
static const uint8_t REGISTER_DMODE = 0x09; // Decode mode: sets BCD code B (0-9, E, H, L, P, and -) or no-decode operation for each digit
static const uint8_t REGISTER_INTENSITY = 0x0A;
static const uint8_t REGISTER_SLIMIT = 0x0B;
static const uint8_t REGISTER_SHUTDOWN = 0x0C;
static const uint8_t REGISTER_DTEST = 0x0F;
// Settings
static const uint8_t DTEST_OFF = 0x00;
static const uint8_t SREEN_LIMIT = MATRIX_WIDTH - 1;
static const uint8_t DMODE_OFF = 0x00;
static const uint8_t INTENSITY_LOW = 0x01;
static const uint8_t SD_DISPLAY_OFF = 0x00;
static const uint8_t SD_DISPLAY_ON = 0x01;
static const uint8_t CLEAR_COL = 0x00;


void ledMatrixInit(ledMatrix_t * ledMatrix, SPI_HandleTypeDef * hSpi, GPIO_TypeDef * csPort, uint16_t csPin){
	// ver de agregar valores de modo, intensidad, etc... !!!!
	ledMatrix->hSpi = hSpi;
	ledMatrix->csPort = csPort;
	ledMatrix->csPin = csPin;

	setPin(ledMatrix->csPort, ledMatrix->csPin); // set CS high to start

	sendCommand(ledMatrix->hSpi, ledMatrix->csPort, ledMatrix->csPin, REGISTER_DTEST, DTEST_OFF);
	sendCommand(ledMatrix->hSpi, ledMatrix->csPort, ledMatrix->csPin, REGISTER_SLIMIT, SREEN_LIMIT);
	sendCommand(ledMatrix->hSpi, ledMatrix->csPort, ledMatrix->csPin, REGISTER_DMODE, DMODE_OFF);
	ledMatrixClear(ledMatrix);

	sendCommand(ledMatrix->hSpi, ledMatrix->csPort, ledMatrix->csPin, REGISTER_INTENSITY, INTENSITY_LOW);
	ledMatrixShutdown(ledMatrix, 1);
}

void ledMatrixShutdown(ledMatrix_t * ledMatrix, uint8_t sd){
	sendCommand(ledMatrix->hSpi, ledMatrix->csPort, ledMatrix->csPin, REGISTER_SHUTDOWN, sd? SD_DISPLAY_OFF : SD_DISPLAY_ON);
}

void ledMatrixClear(ledMatrix_t * ledMatrix){
	for(int i=1; i<=MATRIX_WIDTH; i++){
		sendCommand(ledMatrix->hSpi, ledMatrix->csPort, ledMatrix->csPin, i, CLEAR_COL);
	}
}

void ledMatrixRender(ledMatrix_t * ledMatrix, uint8_t * screen){
	// screen es un array de columnas
    for (int col = 0; col < MATRIX_WIDTH; col++) {
    	sendCommand(ledMatrix->hSpi, ledMatrix->csPort, ledMatrix->csPin, col+1, screen[col]);
    }
}

