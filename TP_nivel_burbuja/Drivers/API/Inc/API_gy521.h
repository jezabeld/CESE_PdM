/*
 * gy521.h
 *
 *  Created on: Apr 6, 2025
 *      Author: jez
 */

#ifndef API_INC_API_GY521_H_
#define API_INC_API_GY521_H_

#include <API_gy521_port.h>
#include "API_delay.h"

typedef struct{
   I2C_HandleTypeDef * hi2c;
   uint8_t devAddress;
} gyro_t;

typedef enum
{
//	SLEEP_MODE 	= 1,
//	STANDBY_MODE,
	LOW_POWER_ACC_MODE,
	LOW_NOISE_ACC_MODE,
//	GYR_MODE,
//	FULL_MODE,
} gyroPowerModes_t;

bool_t gyroInit(gyro_t * gyro, I2C_HandleTypeDef * hi2c, uint8_t devAddress, gyroPowerModes_t mode);

void gyroReadAccel(gyro_t * gyro, int16_t * accX, int16_t * accY, int16_t * accZ);

#endif /* API_INC_API_GY521_H_ */
