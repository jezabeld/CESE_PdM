/*
 * gy521.h
 *
 *  Created on: Apr 6, 2025
 *      Author: jez
 */

#ifndef API_INC_API_GY521_H_
#define API_INC_API_GY521_H_

#include <GY521_port.h>

#define _1G 16384
#define _2G 32768

/**
 * @typedef gyro_t
 * @brief Estructura del giroscopio/acelerometro.
 */
typedef struct{
   I2C_HandleTypeDef * hi2c;
   uint8_t devAddress;
   int16_t calX;
   int16_t calY;
   int16_t calZ;
} gyro_t;

/**
 * @brief Modos de alimentación y funcionamiento del dispositivo.
 * 			Implementados únicamente los modos de acelerómetro.
 */
typedef enum
{
//	SLEEP_MODE 	= 1,
//	STANDBY_MODE,
	LOW_POWER_ACC_MODE,
	LOW_NOISE_ACC_MODE,
//	GYR_MODE,
//	FULL_MODE,
} gyroPowerModes_t;

/**
 * @brief Estados de  la inicialización.
 *
 * `INIT_ERROR` puede indicar errores de comunicación con el dispositvo debido a una dirección errónea
 * 				del mismo o que el dispositivo no es el modelo MPU-6500. Otros modelos requieren
 * 				configuraciones diferentes y no se recomienda el uso de este driver.
 */
typedef enum{
	INIT_ERROR,
	INIT_OK,
} initStatus_t;

/**
 * @brief Inicializa el giroscopio y acelerometro en el modo de alimentación seleccionado.
 *
 * @param gyro Estructura del dispostivo a inicializar.
 * @param hi2c Handler de I2C para la conexión con el dispositivo.
 * @param devAddress Dirección del dispositivo para la conexión por I2C.
 * @param mode Modo de alimentación y funcionamiento seleccionado.
 * @return initStatus_t Estado de la inicialización: `INIT_OK` si la incialización finalizó con éxito,
 * 			`INIT_ERROR` si hubo problemas de conexión o comunicación con el dispositivo.
 */
initStatus_t gyroInit(gyro_t * gyro, I2C_HandleTypeDef * hi2c, uint8_t devAddress, gyroPowerModes_t mode);

/**
 * @brief Lee los valores del acelerómetro.
 *
 * @param gyro Estructura del dispostivo (debe haber sido previamente inicializado).
 * @param accX Puntero a la direccion de memoria donde se guardará el valor leído del eje X.
 * @param accY Puntero a la direccion de memoria donde se guardará el valor leído del eje Y.
 * @param accZ Puntero a la direccion de memoria donde se guardará el valor leído del eje Z.
 */
void gyroReadAccel(gyro_t * gyro, int16_t * accX, int16_t * accY, int16_t * accZ);

#endif /* API_INC_API_GY521_H_ */
