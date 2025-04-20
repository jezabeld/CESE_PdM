/**
 * @file ledMatrix.h
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

#include "ledMatrix_port.h"

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
  * @brief Intensidad de los LEDs de la matriz.
  */
typedef enum
{
	INTENSITY_LOW		= 0x01,
	INTENSITY_MEDIUM	= 0x08,
	INTENSITY_HIGH		= 0x0F,
} ledIntensity_t;

/**
  * @brief Estados de la matriz.
  */
typedef enum{
	MATRIX_ERROR,
	MATRIX_OK,
} matrixStatus_t;

/**
 * @brief Inicializador de la matriz a controlar.
 *
 * Setea el pin definido como Chip Select en HIGH y configura la matriz de leds mediante los registros internos
 * del controlador MAX7219.
 *
 * @param[in,out] ledMatrix Estructura de datos de la matriz.
 * @param[in] hspi Handler de SPI a utilizar para la comunicación.
 * @param[in] csPort Puerto del pin utilizado como CS.
 * @param csPin Número de pin utilizado como CS.
 * @return `MATRIX_OK` si la comunicación se estableció correctamente, `MATRIX_ERROR` en caso contrario.
 */
matrixStatus_t ledMatrixInit(ledMatrix_t * ledMatrix, SPI_HandleTypeDef * hSpi, GPIO_TypeDef * csPort, uint16_t csPin);

/**
 * @brief Setea el nivel de intensidad o brillo de los LEDs de la matriz.
 *
 * @param[in] ledMatrix Estructura de datos de la matriz (debe haber sido inicializada previamente).
 * @param intensity Nivel de intensidad a setear. Los niveles de intensidad permitidos se encuentran definidos en el enum `ledIntensity_t`.
 * @return `MATRIX_OK` si el comando se envió correctamente por SPI, `MATRIX_ERROR` en caso contrario.
 */
matrixStatus_t ledMatrixSetIntensity(ledMatrix_t * ledMatrix, ledIntensity_t intensity);


/**
 * @brief Habilita o deshabilita el modo SHUTDOWN de la matriz.
 *
 * En el modo SHUTDOWN todos los leds se mantienen encendidos y los registros internos del MAX7219 se mantienen inalterados.
 * Este modo puede utilizarse para ahorrar energía o como modo de alarma haciendo parpadear todos los leds entrando y saliendo
 * sucesivamente del modo SD.
 * Durante SHUTDOWN la matriz puede programarse normalmente.
 *
 * @param[in] ledMatrix Estructura de datos de la matriz (debe haber sido inicializada previamente).
 * @param sd `0` para deshabilitar el modo SHUTDOWN, `1` para habilitarlo. Ningún otro valor es admitido.
 * @return `MATRIX_OK` si el comando se envió correctamente por SPI, `MATRIX_ERROR` en caso contrario.
 */
matrixStatus_t ledMatrixShutdown(ledMatrix_t * ledMatrix, uint8_t sd);

/**
 * @brief Limpia la matriz (apaga todos los leds).
 *
 * @param[in] ledMatrix Estructura de datos de la matriz (debe haber sido inicializada previamente).dispositivo
 * @return `MATRIX_OK` si el comando se envió correctamente por SPI, `MATRIX_ERROR` en caso contrario.
 */
matrixStatus_t ledMatrixClear(ledMatrix_t * ledMatrix);

/**
 * @brief Renderiza en la matriz la información del buffer de pantalla.
 *
 * @param[in] ledMatrix Estructura de datos de la matriz (debe haber sido inicializada previamente).
 * @param[in] screen Puntero al buffer de pantalla. Debe ser un array de 8 elementos cada uno correspondiendo a una columna de la matriz.
 * @return `MATRIX_OK` si el comando se envió correctamente por SPI, `MATRIX_ERROR` en caso contrario.
 */
matrixStatus_t ledMatrixRender(ledMatrix_t * ledMatrix, const uint8_t * screen);

#endif /* API_INC_API_LED_MATRIX_H_ */
