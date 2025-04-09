/**
 * @file API_led_matrix.h
 * @brief Device Driver para matriz de LEDs de 8x8 con controlador MAX7219.
 *
 * Creado como parte del Trabajo Práctico final de las materias `Protocolos de
 * 	Comunicación en Sistemas embebidos` y `Programación de Microcontroladores`.
 *
 * @author  Jez
 * @date Mar 25, 2025
 */

#ifndef API_INC_API_LED_MATRIX_H_
#define API_INC_API_LED_MATRIX_H_

#include "API_led_matrix_port.h"

#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8

/**
 * @typedef ledMatrix_t
 * @brief Estructura de la matriz de LEDs a controlar.
 */
typedef struct{
   GPIO_TypeDef * csPort;
   uint16_t csPin;
   SPI_HandleTypeDef * hSpi;
} ledMatrix_t;

/**
  * @brief  Intensidad de los LEDs de la matriz.
  */
typedef enum
{
	INTENSITY_LOW		= 0x01,
	INTENSITY_MEDIUM	= 0x08,
	INTENSITY_HIGH		= 0x0F,
} ledIntensity_t;

/**
 * @brief Inicializador de la matriz a controlar.
 *
 * @param ledMatrix Estructura de datos de la matriz.
 * @param hspi Handler de SPI a utilizar para la comunicación.
 * @param csPort Puerto del pin utilizado como CS.
 * @param csPin Número de pin utilizado como CS.
 */
void ledMatrixInit(ledMatrix_t * ledMatrix, SPI_HandleTypeDef * hSpi, GPIO_TypeDef * csPort, uint16_t csPin);

/**
 * @brief Setea el nivel de intensidad o brillo de los LEDs de la matriz.
 *
 * @param ledMatrix Estructura de datos de la matriz (debe haber sido inicializada previamente).
 * @param intensity Nivel de intensidad a setear.
 */
void ledMatrixSetIntensity(ledMatrix_t * ledMatrix, ledIntensity_t intensity);


/**
 * @brief Habilita o dehabilita el modo SHUTDOWN de la matriz.
 *
 * @param ledMatrix Estructura de datos de la matriz (debe haber sido inicializada previamente).
 * @param sd `0` para deshabilitar el modo SHUTDOWN, `1` para habilitarlo.
 * 			Ningún otro valor es admitido.
 */
void ledMatrixShutdown(ledMatrix_t * ledMatrix, uint8_t sd);

/**
 * @brief Limpia la matriz (apaga todos los leds).
 *
 * @param ledMatrix Estructura de datos de la matriz (debe haber sido inicializada previamente).
 */
void ledMatrixClear(ledMatrix_t * ledMatrix);

/**
 * @brief Renderiza en la matriz la iformación del buffer de pantalla.
 *
 * @param ledMatrix Estructura de datos de la matriz (debe haber sido inicializada previamente).
 * @param screen Puntero al buffer de pantalla. Debe ser un array de 8 elementos cada
 * 			uno correspondiendo a una columna de la matriz.
 */
void ledMatrixRender(ledMatrix_t * ledMatrix, uint8_t * screen);

#endif /* API_INC_API_LED_MATRIX_H_ */
