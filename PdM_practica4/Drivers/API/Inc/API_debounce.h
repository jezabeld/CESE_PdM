/*
 * API_debounce.h
 *
 *  Created on: Mar 27, 2025
 *      Author: jez
 */

#ifndef API_INC_API_DEBOUNCE_H_
#define API_INC_API_DEBOUNCE_H_

#include <stdint.h>
#include <stdbool.h>


typedef enum{
	BUTTON_UP,
	BUTTON_FALLING,
	BUTTON_DOWN,
	BUTTON_RISING,
} debounceState_t;

void debounceFSM_init(void);		// debe cargar el estado inicial
void debounceFSM_update(void);	// debe leer las entradas, resolver la lógic transición de estados y actualizar las salidas

void buttonPressed(void);			// debe encender el LED
void buttonReleased(void);		// debe apagar el LED

#endif /* API_INC_API_DEBOUNCE_H_ */
