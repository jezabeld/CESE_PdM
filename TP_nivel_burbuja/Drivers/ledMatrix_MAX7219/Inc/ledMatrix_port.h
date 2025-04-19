/**
 * @file API_led_matrix_port.h
 * @brief Device Driver para matriz de LEDs de 8x8 con controlador MAX7219.
 *
 * Creado como parte del Trabajo Práctico final de las materias `Protocolos de
 * 	Comunicación en Sistemas embebidos` y `Programación de Microcontroladores`.
 *
 * @author  Jez
 * @date Mar 25, 2025
 */

#ifndef API_INC_API_LED_MATRIX_PORT_H_
#define API_INC_API_LED_MATRIX_PORT_H_

#include "stdint.h"
#include "stm32f4xx_hal.h"

/**
 * @brief Setea un pin a HIGH.
 *
 * @param port Puerto del pin a setear.
 * @param pin Número de pin a setear.
 */
void setPin(GPIO_TypeDef * port, uint16_t pin);

/**
 * @brief Resetea un pin a LOW.
 *
 * @param port Puerto del pin a resetear.
 * @param pin Número de pin a resetear.
 */
void clearPin(GPIO_TypeDef * port, uint16_t pin);

/**
 * @brief Envía un comando por SPI con su respectivo valor, habilitando el device mediante el
 * 	pin designado como CS (chip select).
 *
 * @param hspi Puntero al handler de SPI a utilizar.
 * @param port Puerto del pin utilizado como CS.
 * @param pin Número de pin del CS.
 * @param command Conamdo a enviar (8 bits iniciales).
 * @param value Valor del comando a enviar (siguientes 8 bits).
 */
void sendCommand(SPI_HandleTypeDef * hspi, GPIO_TypeDef * port, uint16_t pin, uint8_t command, uint8_t value);

#endif /* API_INC_API_LED_MATRIX_PORT_H_ */
