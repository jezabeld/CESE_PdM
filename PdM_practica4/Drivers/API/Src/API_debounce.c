/*
 * API_debounce.c
 *
 *  Created on: Mar 27, 2025
 *      Author: jez
 */

#include <API_debounce.h>
#include <API_delay.h>

#define DEBOUNCE_TIME 40
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA

static debounceState_t FSMstate;
static delay_t debounceDelay;

void debounceFSM_init(void){
	FSMstate = BUTTON_UP;
	delayInit(&debounceDelay, DEBOUNCE_TIME);
}
void debounceFSM_update(){
	switch(FSMstate){
	case BUTTON_UP:
		if(HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)){ // si detecto un 0 (posible presion de boton)
			FSMstate = BUTTON_FALLING;
			buttonReleased();
			delayRead(&debounceDelay); // pongo a correr el timer
		}
		break;
	case BUTTON_FALLING:
		if(delayRead(&debounceDelay) && HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)){ // si termino el delay y efectivamente esta presionado
			FSMstate = BUTTON_DOWN;
		}
		break;
	case BUTTON_DOWN:
		if(!HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)){ // si detecto un 1 (posible release del boton)
			FSMstate = BUTTON_RISING;
			buttonPressed();
			delayRead(&debounceDelay); // pongo a correr el timer
		}
		break;
	case BUTTON_RISING:
		if(delayRead(&debounceDelay) && !HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)){ // si termino el delay y efectivamente se solto el boton
			FSMstate = BUTTON_UP;
		}
		break;
	default:
		debounceFSM_init();
	}
}

void buttonPressed(void){
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
}
void buttonReleased(void){
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
}
