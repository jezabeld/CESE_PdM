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
/* USER CODE BEGIN Includes */
#include "API_debounce.h"
#include "API_delay.h"
#include "API_uart.h"
#include "API_gy521.h"
#include "API_led_matrix.h"
#include "math.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
// Dimensiones de la matriz LED
#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8
#define MOVEMENT_SENSIBILITY 2.0
#define TIEMPO_RENDER 500
#define DOT_SIZE 1

// Matriz buffer de pantalla
uint8_t DISPLAY[MATRIX_HEIGHT] = {0};

// Posición del dot en la pantalla
uint8_t offsetX = 3;
uint8_t offsetY = 3;

ledMatrix_t myMatrix;
gyro_t myGyro;
const uint8_t gyroAddress = 0x68;
bool_t gyroStatus;
int16_t accelX, accelY, accelZ;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
//static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

void mapAccelToPosition(int16_t accX, int16_t accY, uint8_t * posX, uint8_t * posY);
int32_t capValue(int32_t value, int32_t min, int32_t max);
int32_t mapValues(int32_t value, int32_t fromLow, int32_t fromHigh, int32_t toLow, int32_t toHigh);
int32_t mapTanH(int32_t value, int32_t fromLow, int32_t fromHigh, int32_t toLow, int32_t toHigh);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void drawMatrix(void);
void moveLeft(void);
void moveRight(void);
void moveForward(void);
void moveBackward(void);

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	//uint8_t buffer[2];
	uint8_t smiley[] =          // fill array for the smiley
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
	gyroStatus = gyroInit(&myGyro, &hi2c1, gyroAddress, LOW_NOISE_ACC_MODE);
	ledMatrixInit(&myMatrix, &hspi1, GPIOB, GPIO_PIN_6);

	//
	buttonChange_t btnChange;
	debounceFSM_init();

	//
	delay_t myDelay;
    delayInit(&myDelay, TIEMPO_RENDER);


  	// graficar escena inicial
    ledMatrixRender(&myMatrix, smiley);


    HAL_Delay(1500);


    // render posicion inicial
    drawMatrix();
    uartSendString(gyroStatus ? "Connected\r\n" : "Not connected\r\n");

    //HAL_Delay(500);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  btnChange = debounceFSM_update();

	  if(btnChange == BUTTON_PRESSED){
		  uartSendString("Flanco descendente detectado\r\n");
		  moveRight();
	  }
	  if(btnChange == BUTTON_RELEASED){
		  uartSendString("Flanco ascendente detectado\r\n");
		  moveLeft();
	  }

	  if(delayRead(&myDelay)){
		  uartSendString("Starting...\r\n");
		  gyroReadAccel(&myGyro, &accelX, &accelY, &accelZ);
		  mapAccelToPosition(accelX, accelY, &offsetX, &offsetY);
		  uartSendString("Acc X: ");
		  uartSendValue(accelX);
		  uartSendString(", Acc Y: ");
		  uartSendValue(accelY);
		  uartSendString(", Acc Z: ");
		  uartSendValue(accelZ);
		  uartSendString(", pos X: ");
		  uartSendValue(offsetX);
		  uartSendString(", pos Y: ");
		  uartSendValue(offsetY);
		  uartSendString("\r\n");
		  drawMatrix();
	  }

	  //HAL_Delay(200);

	  // Simulación de movimiento
	  /*for(uint8_t i=0; i<5; i++){
		  moveRight();
		  uartSendString("Pos X: ");
		  uartSendValue(offsetX);
		  uartSendString(", Pos Y: ");
		  uartSendValue(offsetY);
		  uartSendString("\r\n");
		  HAL_Delay(500);
	  }
	  for(uint8_t i=0; i<5; i++){
		  moveForward();
		  uartSendString("Pos X: ");
		  uartSendValue(offsetX);
		  uartSendString(", Pos Y: ");
		  uartSendValue(offsetY);
		  uartSendString("\r\n");
		  HAL_Delay(500);
	  }
	  for(uint8_t i=0; i<5; i++){
		  moveLeft();
		  uartSendString("Pos X: ");
		  uartSendValue(offsetX);
		  uartSendString(", Pos Y: ");
		  uartSendValue(offsetY);
		  uartSendString("\r\n");
		  HAL_Delay(500);
	  }
	  for(uint8_t i=0; i<5; i++){
		  moveBackward();
		  uartSendString("Pos X: ");
		  uartSendValue(offsetX);
		  uartSendString(", Pos Y: ");
		  uartSendValue(offsetY);
		  uartSendString("\r\n");
		  HAL_Delay(500);
	  }*/

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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
/*static void MX_USART2_UART_Init(void)
{


  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

}*/

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
/*********************************************************************************************************************/

// Función para armar el buffer de pantalla
void drawMatrix(void) {
	for (int col = 0; col < MATRIX_WIDTH; col++) {
		uint8_t display_column = 0;

		for (int row = 0; row < MATRIX_HEIGHT; row++) {
			if ((row >= offsetY) && (row <= (offsetY + DOT_SIZE-1)) && (col >= offsetX) && (col <= (offsetX + DOT_SIZE-1))) {
				display_column |= (1 << row); // Enciende el LED si es la posicion X,Y
			}

		}
		DISPLAY[col] = display_column;
	}
	ledMatrixRender(&myMatrix, DISPLAY);
}

// Simulación del movimiento
void moveLeft(void) {
    offsetX = (offsetX - 1 + MATRIX_WIDTH) % MATRIX_WIDTH;
    drawMatrix();
}
void moveRight(void) {
    offsetX = (offsetX + 1 + MATRIX_WIDTH) % MATRIX_WIDTH;
    drawMatrix();
}/*
void moveForward(void) {
    offsetY = (offsetY + 1 + MATRIX_HEIGHT) % MATRIX_HEIGHT;
    drawMatrix();
}
void moveBackward(void) {
    offsetY = (offsetY - 1 + MATRIX_HEIGHT) % MATRIX_HEIGHT;
    drawMatrix();
}*/

// map acelerometro a matriz de leds
void mapAccelToPosition(int16_t accX, int16_t accY, uint8_t * posX, uint8_t * posY){
	accX = capValue(accX, -_1G, _1G);
	accY = capValue(accY, -_1G, _1G);
	// mapeo lineal
	// *posY = mapValues(accX, -_1G, _1G, 0, MATRIX_WIDTH-1);
	// *posX = mapValues(accY, -_1G, _1G, 0, MATRIX_HEIGHT-1);
	// mapeo con tangente hiperbolica
	*posY = mapTanH(accX, -_1G, _1G, 0, MATRIX_WIDTH);
	*posX = mapTanH(accY, -_1G, _1G, 0, MATRIX_HEIGHT);
}

int32_t capValue(int32_t value, int32_t min, int32_t max){
	return (value > max)? max : ((value < min)? min : value);
}
int32_t mapValues(int32_t value, int32_t fromLow, int32_t fromHigh, int32_t toLow, int32_t toHigh){
	return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}
int32_t mapTanH(int32_t value, int32_t fromLow, int32_t fromHigh, int32_t toLow, int32_t toHigh){
	double norm = (double)value / fromHigh; // -1.0 a 1.0
	double curved = (tanh(norm * MOVEMENT_SENSIBILITY) + 1.0) / 2.0; // 0.0 a 1.0
	int32_t output = (int32_t)(curved * (toHigh - toLow) + toLow);
	return output;
}
/*************************************************************************************************************************/
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
