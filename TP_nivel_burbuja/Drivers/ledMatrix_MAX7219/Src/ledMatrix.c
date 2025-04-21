/**
 * @file API_led_matrix.c
 * @brief Device Driver para matriz de LEDs de 8x8 con controlador MAX7219.
 *
 * Creado como parte del Trabajo Práctico final de las materias `Protocolos de
 * 	Comunicación en Sistemas embebidos` y `Programación de Microcontroladores`.
 *
 * @author  Jez
 * @date Mar 25, 2025
 */

#include "assert.h"
#include "ledMatrix.h"

// Driver registers
static const uint8_t REGISTER_DMODE = 0x09; ///< Decode mode: sets BCD code B (0-9, E, H, L, P, and -) or no-decode operation for each digit
static const uint8_t REGISTER_INTENSITY = 0x0A;
static const uint8_t REGISTER_SLIMIT = 0x0B;
static const uint8_t REGISTER_SHUTDOWN = 0x0C;
static const uint8_t REGISTER_DTEST = 0x0F;
// Settings
static const uint8_t DTEST_OFF = 0x00;
static const uint8_t SREEN_LIMIT = MATRIX_WIDTH - 1;
static const uint8_t DMODE_OFF = 0x00;
static const uint8_t SD_DISPLAY_OFF = 0x00;
static const uint8_t SD_DISPLAY_ON = 0x01;
static const uint8_t CLEAR_COL = 0x00;

matrixStatus_t ledMatrixInit(ledMatrix_t * ledMatrix, SPI_HandleTypeDef * hSpi, GPIO_TypeDef * csPort, uint16_t csPin){
	assert(ledMatrix);
	assert(hSpi);
	assert(csPort);
	assert_param(csPin >= 0 && csPin <16);

	ledMatrix->hSpi = hSpi;
	ledMatrix->csPort = csPort;
	ledMatrix->csPin = csPin;

	setPin(ledMatrix->csPort, ledMatrix->csPin); // set CS high to start
	bool status; // variable de estatus de comunicacion

	status = sendCommand(ledMatrix->hSpi, ledMatrix->csPort, ledMatrix->csPin, REGISTER_DTEST, DTEST_OFF); // test mode OFF

	status |= sendCommand(ledMatrix->hSpi, ledMatrix->csPort, ledMatrix->csPin, REGISTER_SLIMIT, SREEN_LIMIT); // control 8 columns
	status |= sendCommand(ledMatrix->hSpi, ledMatrix->csPort, ledMatrix->csPin, REGISTER_DMODE, DMODE_OFF); // decoder OFF
	status |= (ledMatrixClear(ledMatrix) == MATRIX_ERROR)? true : false;

	status |= sendCommand(ledMatrix->hSpi, ledMatrix->csPort, ledMatrix->csPin, REGISTER_INTENSITY, INTENSITY_LOW);

	status |= (ledMatrixShutdown(ledMatrix, 0) == MATRIX_ERROR)? true : false; // salir del modo SHUTDOWN en el que inicia por default

	return (status? MATRIX_ERROR : MATRIX_OK);
}

matrixStatus_t ledMatrixShutdown(ledMatrix_t * ledMatrix, uint8_t sd){
	assert(ledMatrix);
	assert(ledMatrix->hSpi); // verifica que la estructura fue inicializada previamente
	assert_param((sd == 0) || (sd == 1)); // valores válidos

	bool status = sendCommand(ledMatrix->hSpi, ledMatrix->csPort, ledMatrix->csPin, REGISTER_SHUTDOWN, sd? SD_DISPLAY_OFF : SD_DISPLAY_ON);
	return (status? MATRIX_ERROR : MATRIX_OK);
}

matrixStatus_t ledMatrixClear(ledMatrix_t * ledMatrix){
	assert(ledMatrix);
	assert(ledMatrix->hSpi); // verifica que la estructura fue inicializada previamente

	for(int i=1; i<=MATRIX_WIDTH; i++){
		if(sendCommand(ledMatrix->hSpi, ledMatrix->csPort, ledMatrix->csPin, i, CLEAR_COL)) return MATRIX_ERROR;
	}
	return MATRIX_OK;
}

matrixStatus_t ledMatrixRender(ledMatrix_t * ledMatrix, const uint8_t * screen){
	assert(ledMatrix);
	assert(ledMatrix->hSpi); // verifica que la estructura fue inicializada previamente
	assert_param(sizeof(screen) == MATRIX_WIDTH); // screen es un array de columnas

    for (int col = 0; col < MATRIX_WIDTH; col++) {
    	if(sendCommand(ledMatrix->hSpi, ledMatrix->csPort, ledMatrix->csPin, col+1, screen[col])) return MATRIX_ERROR;
    }
    return MATRIX_OK;
}

matrixStatus_t ledMatrixSetIntensity(ledMatrix_t * ledMatrix, ledIntensity_t intensity){
	assert(ledMatrix);
	assert(intensity);

	bool status = sendCommand(ledMatrix->hSpi, ledMatrix->csPort, ledMatrix->csPin, REGISTER_INTENSITY, intensity);
	return (status? MATRIX_ERROR : MATRIX_OK);
}
