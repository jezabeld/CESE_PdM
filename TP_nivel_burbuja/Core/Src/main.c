/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes *//************************************************/

#include "API_delay.h"
#include "API_uart.h"
#include "API_debounce.h"
#include "GY521.h"
#include "ledMatrix.h"
#include "math.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/**
 * @typedef bubbleState_t
 * @brief Estados de la FSM del nivel de burbuja.
 */
typedef enum{
	MEDICION_SIMPLE,
	COMPARACION_NIVELES,
} bubbleState_t;


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD *//******************************************************/
#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8
#define NOT_VALID_POS 8
#define MAP_MODE_TANH 1
#define MOVEMENT_SENSIBILITY 2.0 ///< Sensibilidad del movimiento de la burbuja (valores posibles: 1.5, 2.0, 2.5, 3.0)
#define INIT_TIME 2000 			///< Tiempo en la pantalla de binvenida al iniciar la aplicación.
#define RENDER_TIME 500 		///< Tiempo entre mediciones.

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV *//******************************************************/
// Parámetros de la matriz
ledMatrix_t myMatrix;
uint8_t DISPLAY[MATRIX_HEIGHT] = {0}; 	///< Buffer de pantalla.
const uint8_t smileyWelcome[] =			///< Imagen de la pantalla de inicio.
	{
	0b00111110,
	0b01000001,
	0b10010101,
	0b10100001,
	0b10100001,
	0b10010101,
	0b01000001,
	0b00111110
};

// parametros del botón
buttonChange_t btnChange;

// Parámetros del acelerómetro
const uint8_t gyroAddress = 0x68;	///< Dirección SPI del GY-521 conectado.
gyro_t myGyro;
bool_t gyroStatus;
int16_t accelX, accelY, accelZ;

// parámetros para la aplicación
bubbleState_t estadoBurbuja;
gyroStatus_t estadoInitNB;
delay_t bubbleDelay;
// Posición del dot en la pantalla
uint8_t posX;
uint8_t posY;
uint8_t posFijaX;
uint8_t posFijaY;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
//static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP *//*****************************************************/

/**
 * @brief Inicializa la FSM del nivel de burbuja.
 *
 * Establece el estado inicial en `MEDICION_SIMPLE` e inicializa los periféricos necesarios
 * (acelerómetro, matriz display y el botón utilizado para cambiar de modo).
 *
 * @return Estado de inicialización: `INIT_OK` si fue exitosa, `INIT_ERROR` si algún periférico
 * no inicializó correctamente.
 */
gyroStatus_t bubbleInit(void);

/**
 * @brief Actualiza la FSM del nivel de burbuja.
 *
 * Detecta cambios en las entradas (acelerómetro y botón) y define el estado y las salidas de la FSM.
 *
 * @return Estado de la FSM del nivel de burbuja.
 */
bubbleState_t bubbleUpdate(void);

/**
 * @brief Guarda la posición actual de la burbuja para utilizarla para el modo de Comparación de Niveles.
 *
 * @param posX Posición de la burbuja en el eje X de la matrix.
 * @param posY Posición de la burbuja en el eje Y de la matrix.
 * @param[out] posFX Puntero donde guardar la posición de la burbuja en el eje X de la matriz.
 * @param[out] posFY Puntero donde guardar la posición de la burbuja en el eje Y de la matriz.
 */
void saveCurrentLevel(uint8_t posX, uint8_t posY, uint8_t * posFX, uint8_t * posFY);

/**
 * @brief Descarta la posición guardada de la burbuja para el modo de Comparación de Niveles.
 *
 * Para descartar la posición guardada, reemplaza ambos valores en X e Y por un valor de posición
 * no válida en la matriz, definido en la macro `NOT_VALID_POS`. Este valor es reconocido por la
 * función `drawMatrix` para identificar que no se guardó una posición fija válida para representar
 * en la matriz de leds.
 *
 * @param[out] posFX Puntero a donde se guardó la posición de la burbuja en el eje X de la matriz.
 * @param[out] posFY Puntero a donde se guardó la posición de la burbuja en el eje Y de la matriz.
 */
void deleteSavedLevel(uint8_t * posFX, uint8_t * posFY);

/**
 * @brief Genera el buffer de pantalla a partir de la posición (o posiciones) especificada.
 *
 * Esta función identifica los leds que necesitan encenderse para representar la posición de la burbuja
 * definida. También identifica si existe una posición fija para representar, y de ser así, también será
 * representada en el buffer de pantalla generado.
 * Al finalizar el armado del buffer de pantalla, llamará a las funciones correspondientes para renderizar
 * el buffer de pantalla en la matrix y enviar los valores actuales de acelerómetro y posiciones por la UART.
 *
 * @param posX Posición de la burbuja en el eje X de la matrix.
 * @param posY Posición de la burbuja en el eje Y de la matrix.
 * @param posFX Posición de la burbuja fija en el eje X de la matrix (en modo Comparación de Niveles).
 * @param posFY Posición de la burbuja fija en el eje Y de la matrix (en modo Comparación de Niveles).
 */
void drawMatrix(const uint8_t posX, const uint8_t posY, const uint8_t posFX, const uint8_t posFY);

/**
 * @brief Realiza el mapping entre los valores obtenidos del acelerómetro y la posición de la burbuja.
 *
 * Para el mapeo se tienen en cuenta solamente los valores correspondientes a la aceleración de la gravedad
 * (determinación de la posición únicamente) descartando valores mayores a 1G posiblemente correspondientes
 * a aceleraciones debidas a otros movimientos.
 *
 * Esta función invierte los ejes X e Y del acelerómetro respecto a las salidas en la matriz de leds para que coincida
 * con la forma en la que se visualiza la matriz en la aplicación desarrollada.
 *
 * @param accX Valor de lectura de la aceleración en el eje X del módulo GY-521.
 * @param accY Valor de lectura de la aceleración en el eje Y del módulo GY-521.
 * @param accZ Valor de lectura de la aceleración en el eje Z del módulo GY-521.
 * @param[out] posX Puntero donde retornar la posición de la burbuja en el eje X de la matriz.
 * @param[out] posY Puntero donde retornar la posición de la burbuja en el eje Y de la matriz.
 * @param mode Modo de mapeo: 0-Mapeo lineal, 1-Mapeo no lineal.{
 */
void mapAccelToPosition(int16_t accX, int16_t accY, uint8_t * posX, uint8_t * posY, uint8_t mode);

/**
 * @brief Función auxiliar para acotar valores.
 *
 * Acota un valor a los límites superior e inferior determinados.
 *
 * @param value Valor a acotar.
 * @param min Cota inferior.
 * @param max Cota superior.
 * @return Valor acotado.
 */
int32_t capValue(int32_t value, int32_t min, int32_t max);

/**
 * @brief Función auxiliar para mapeo lineal de valores.
 *
 * Realiza una transformación lineal de un valor que se encuetra entre dos limites de entrada al valor
 * correspondiente entre los límites de salida especificados.
 *
 * @param value Valor a transformar.
 * @param fromLow Límite inferior del valor de entrada.
 * @param fromHigh Límite superior del valor de entrada.
 * @param toLow Límite inferior del valor de salida.
 * @param toHigh Límite superior del valor de salida.
 * @return Valor transformado.
 */
int32_t mapValues(int32_t value, int32_t fromLow, int32_t fromHigh, int32_t toLow, int32_t toHigh);

/**
 * @brief Función auxiliar para mapeo no lineal de valores.
 *
 * Realiza una transformación no lineal de un valor que se encuetra entre dos limites de entrada al valor
 * correspondiente entre los límites de salida especificados utilizando la función Tangente Hiperbólica.
 *
 * @param value Valor a transformar.
 * @param fromLow Límite inferior del valor de entrada.
 * @param fromHigh Límite superior del valor de entrada.
 * @param toLow Límite inferior del valor de salida.
 * @param toHigh Límite superior del valor de salida.
 * @return Valor transformado.
 */
int32_t mapTanH(int32_t value, int32_t fromLow, int32_t fromHigh, int32_t toLow, int32_t toHigh);

/**
 * @brief Envía los valores de entrada y salida por la UART.
 *
 * Esta función envia el estado actual de la FSM del nivel de burbuja (Medición Simple o Comparación de
 * Niveles) junto a los valores leídos del acelerómetro y los valores calculados de posición de las burbujas
 * móvil y fija.
 */
void sendOutputsByUart(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  //MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  /* MAIN USER CODE ***********************************************************/
	uartInit();
	estadoInitNB = bubbleInit();
    uartSendString((estadoInitNB == GYRO_OK) ? "Nivel de Burbuja inicializado\r\n" : "Error de inicialización\r\n");


	// delay de renderizado
    delayInit(&bubbleDelay, RENDER_TIME);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  estadoBurbuja = bubbleUpdate();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/******************************************************************************/

gyroStatus_t bubbleInit(void){
	estadoBurbuja = MEDICION_SIMPLE;
	posFijaX = NOT_VALID_POS;
	posFijaY = NOT_VALID_POS;

	gyroStatus_t gyroStatus = gyroInit(&myGyro, &hi2c1, gyroAddress, LOW_NOISE_ACC_MODE);
	matrixStatus_t lmStatus = ledMatrixInit(&myMatrix, &hspi1, GPIOB, GPIO_PIN_6);
	debounceFSM_init();

	if(gyroStatus == GYRO_ERROR || lmStatus == MATRIX_ERROR){
		return GYRO_ERROR;
	}
	ledMatrixClear(&myMatrix);
	ledMatrixRender(&myMatrix, smileyWelcome);
	HAL_Delay(INIT_TIME); // tiempo de la pantalla de inicio
	return GYRO_OK;
}

bubbleState_t bubbleUpdate(void){
	btnChange = debounceFSM_update();

	switch(estadoBurbuja){
	case MEDICION_SIMPLE:
		if(btnChange == BUTTON_PRESSED){
			saveCurrentLevel(posX, posY, &posFijaX, &posFijaY);
			estadoBurbuja = COMPARACION_NIVELES;
		}
		break;
	case COMPARACION_NIVELES:
		if(btnChange == BUTTON_PRESSED){
			deleteSavedLevel(&posFijaX, &posFijaY);
			estadoBurbuja = MEDICION_SIMPLE;
		}
		break;
	default:
		bubbleInit();
	}

	// Salidas:
	if(delayRead(&bubbleDelay)){
		gyroReadAccel(&myGyro, &accelX, &accelY, &accelZ);
		mapAccelToPosition(accelX, accelY, &posX, &posY, MAP_MODE_TANH);
		drawMatrix(posX, posY, posFijaX, posFijaY);
	}

	return estadoBurbuja;
}

void drawMatrix(const uint8_t posX, const uint8_t posY, const uint8_t posFX, const uint8_t posFY) {
	assert_param((posX >= 0) && (posX < MATRIX_WIDTH));
	assert_param((posY >= 0) && (posY < MATRIX_HEIGHT));
	assert_param((posFX >= 0) && (posFX <= MATRIX_WIDTH));
	assert_param((posFY >= 0) && (posFY <= MATRIX_HEIGHT));

	for (int col = 0; col < MATRIX_WIDTH; col++) {
		uint8_t display_column = 0;

		for (int row = 0; row < MATRIX_HEIGHT; row++) {
			if ((row == posY) && (col == posX)) {
				display_column |= (1 << row); // Enciende el LED si es la posicion X,Y definida
			}
			// posicion fija si hay
			if(posFX != NOT_VALID_POS && posFY != NOT_VALID_POS){
				if ((row == posFY) && (col == posFX)) {
					display_column |= (1 << row);
				}
			}
		}
		DISPLAY[col] = display_column;
	}
	ledMatrixRender(&myMatrix, DISPLAY);
	sendOutputsByUart();
}

void saveCurrentLevel(uint8_t posX, uint8_t posY, uint8_t * posFX, uint8_t * posFY){
	assert_param((posX >= 0) && (posX < MATRIX_WIDTH));
	assert_param((posY >= 0) && (posY < MATRIX_HEIGHT));
	assert(posFX);
	assert(posFY);

	*posFX = posX;
	*posFY = posY;
}
void deleteSavedLevel(uint8_t * posFX, uint8_t * posFY){
	assert(posFX);
	assert(posFY);

	*posFX = NOT_VALID_POS;
	*posFY = NOT_VALID_POS;
}

void mapAccelToPosition(int16_t accX, int16_t accY, uint8_t * posX, uint8_t * posY, uint8_t mode){
	assert_param((accX >= -_2G) && (accX <= _2G));
	assert_param((accY >= -_2G) && (accY <= _2G));
	assert(posX);
	assert(posY);
	assert_param((mode == 0) || (mode == 1)); // modos validos

	accX = capValue(accX, -_1G, _1G); // tengo en cuenta solo los valores de la aceleracion de la gravedad
	accY = capValue(accY, -_1G, _1G);

	if(!mode){ // mapeo lineal
		*posY = mapValues(accX, -_1G, _1G, 0, MATRIX_WIDTH-1);
		*posX = mapValues(accY, -_1G, _1G, 0, MATRIX_HEIGHT-1);
	} else { // mapeo con tangente hiperbolica
		*posY = mapTanH(accX, -_1G, _1G, 0, MATRIX_WIDTH);
		*posX = mapTanH(accY, -_1G, _1G, 0, MATRIX_HEIGHT);
	}
}

int32_t capValue(int32_t value, int32_t min, int32_t max){
	return (value > max)? max : ((value < min)? min : value);
}
int32_t mapValues(int32_t value, int32_t fromLow, int32_t fromHigh, int32_t toLow, int32_t toHigh){
	return (int32_t)((value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow);
}
int32_t mapTanH(int32_t value, int32_t fromLow, int32_t fromHigh, int32_t toLow, int32_t toHigh){
	double norm = (double)value / fromHigh; // -1.0 a 1.0
	double curved = (tanh(norm * MOVEMENT_SENSIBILITY) + 1.0) / 2.0; // 0.0 a 1.0
	return (int32_t)(curved * (toHigh - toLow) + toLow);
}

void sendOutputsByUart(void){
	if(estadoBurbuja == MEDICION_SIMPLE){
		  uartSendString("Estado: MEDICION SIMPLE \r\n");
	  } else {
		  uartSendString("Estado: COMPARACION NIVELES \r\n");
	  }

	  uartSendString("Acc X: "); 	uartSendValue(accelX);
	  uartSendString(", Acc Y: "); 	uartSendValue(accelY);
	  uartSendString(", Acc Z: "); 	uartSendValue(accelZ);
	  uartSendString(", pos X: "); 	uartSendValue(posX);
	  uartSendString(", pos Y: ");	uartSendValue(posY);
	  uartSendString(", pos FX: ");	uartSendValue(posFijaX);
	  uartSendString(", pos FY: ");	uartSendValue(posFijaY);
	  uartSendString("\r\n");

}
/******************************************************************************/
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
