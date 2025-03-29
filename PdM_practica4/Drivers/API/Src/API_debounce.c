/**
 * @file API_debounce.c
 * @brief Implementación de una máquina de estados finitos para el debounce de un botón utilizando retardos no bloqueantes.
 *
 * @author Jez
 * @date Mar 27, 2025
 */


#include <API_debounce.h>

#define DEBOUNCE_TIME 40
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA

static debounceState_t FSMstate;
static delay_t debounceDelay;
static bool_t buttonWasPressed = false;

/**
 * @brief Callback que se ejecuta cuando se detecta una pulsación del botón.
 *
 * Marca la variable `buttonWasPressed` como verdadera para indicar que el botón fue presionado.
 */

void static buttonPressed(void){
	buttonWasPressed = true;
}

/**
 * @brief Inicializa la FSM de debounce.
 *
 * Establece el estado inicial en `BUTTON_UP` y configura el temporizador de debounce.
 */
void debounceFSM_init(void){
	FSMstate = BUTTON_UP;
	delayInit(&debounceDelay, DEBOUNCE_TIME);
}

/**
 * @brief Actualiza la FSM de debounce.
 *
 * Detecta cambios en el estado del botón aplicando un filtro de debounce basado en retardos.
 */
void debounceFSM_update(){
	switch(FSMstate){
	case BUTTON_UP:
		if(!HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)){ // si detecto un 0 (posible presion de boton - boton con pullup resistor)
			FSMstate = BUTTON_FALLING;
			delayRead(&debounceDelay); // pongo a correr el timer
		}
		break;
	case BUTTON_FALLING:
		if(delayRead(&debounceDelay) && !HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)){ // si termino el delay y efectivamente esta presionado
			FSMstate = BUTTON_DOWN;
			buttonPressed();
		}
		break;
	case BUTTON_DOWN:
		if(HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)){ // si detecto un 1 (posible release del boton - boton con pullup resistor)
			FSMstate = BUTTON_RISING;
			delayRead(&debounceDelay); // pongo a correr el timer
		}
		break;
	case BUTTON_RISING:
		if(delayRead(&debounceDelay) && HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)){ // si termino el delay y efectivamente se solto el boton
			FSMstate = BUTTON_UP;
		}
		break;
	default:
		debounceFSM_init();
	}
}

/**
 * @brief Lee una variable indicando si el botón fue presionado desde la ultima lectura y luego la reinicia.
 *
 * NOTA: solo contempla que el botón se haya presionado, pero no necesariamente que haya sido soltado.
 *
 * @return `true` si el botón fue presionado desde la última consulta, `false` en caso contrario.
 */
bool_t readKey(void){
	bool_t returnVal = buttonWasPressed;
	buttonWasPressed = false;
	return returnVal;
}


//void static buttonReleased(void){
	// no implementada
//}
