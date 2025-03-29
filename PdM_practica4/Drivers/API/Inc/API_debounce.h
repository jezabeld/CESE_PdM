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
#include <API_delay.h>

typedef enum{
	BUTTON_UP,
	BUTTON_FALLING,
	BUTTON_DOWN,
	BUTTON_RISING,
} debounceState_t;

void debounceFSM_init(void);		// debe cargar el estado inicial
void debounceFSM_update(void);	// debe leer las entradas, resolver la lógic transición de estados y actualizar las salidas

/* La función readKey debe leer una variable interna del módulo y devolver true o false si la tecla fue presionada.
Si devuelve true, debe resetear (poner en false) el estado de la variable.*/
bool_t readKey(void);

#endif /* API_INC_API_DEBOUNCE_H_ */
