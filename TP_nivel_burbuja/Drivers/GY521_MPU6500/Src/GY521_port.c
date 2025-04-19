/*
 * @file GY521_port.c
 * @brief Device driver para el acelerómetro del módulo GY-521 con controlador MPU-6500.
 *
 * Creado como parte del Trabajo Práctico final de las materias `Protocolos de
 * 	Comunicación en Sistemas embebidos` y `Programación de Microcontroladores`.
 *
 * @author  Jez
 * @date Apr 6, 2025
 */

#include "GY521_port.h"


void readRegister(I2C_HandleTypeDef * hi2c, uint8_t devAddress, uint8_t * data, uint8_t regAddress, uint8_t sizeRead){
	HAL_I2C_Mem_Read(hi2c, devAddress<<1, regAddress, 1, data, sizeRead, HAL_MAX_DELAY);
}

void writeRegister(I2C_HandleTypeDef * hi2c, uint8_t devAddress, uint8_t * data, uint8_t regAddress){
	HAL_I2C_Mem_Write(hi2c, devAddress<<1, regAddress, 1, data, 1, HAL_MAX_DELAY);
}
