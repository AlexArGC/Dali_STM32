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
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RX_BUFFER_SIZE 10
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t rxBuffer[RX_BUFFER_SIZE];
volatile uint8_t dataReceived = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  const char *str_tx = "hello ";
  memset(rxBuffer, 0, RX_BUFFER_SIZE);

  // Сброс флагов ошибок UART перед началом работы
  __HAL_UART_CLEAR_OREFLAG(&huart1);
  __HAL_UART_CLEAR_FEFLAG(&huart1);
  __HAL_UART_CLEAR_NEFLAG(&huart1);
  __HAL_UART_CLEAR_PEFLAG(&huart1);

  if (HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rxBuffer, RX_BUFFER_SIZE) != HAL_OK)
  {
    Error_Handler();
  }
  // Отключаем Half‑Transfer Interrupt (HT), чтобы игнорировать событие заполнения половины буфера
  __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* передаём строку */
    if (HAL_UART_Transmit(&huart1, (uint8_t *)str_tx, strlen(str_tx), 100) != HAL_OK)
    {
      /* ошибка при передаче */
      Error_Handler();
    }

    /* пауза 1 секунду перед следующей отправкой */
    HAL_Delay(2000);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
  if (huart->Instance == USART1)
  {
    dataReceived = 1;
    HAL_UART_Transmit(&huart1, rxBuffer, size, 200);

    // Перезапуск приёма
    if (HAL_UARTEx_ReceiveToIdle_DMA(huart, rxBuffer, RX_BUFFER_SIZE) != HAL_OK)
    {
      // Если не удалось запустить приём — обработать ошибку
      Error_Handler();
    }
    // Снова отключаем прерывание HT
    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->ErrorCode & HAL_UART_ERROR_ORE)
  {
    // Абортируем DMA-приём — аппаратно выключаем канал, чтобы он не успел записать следующий байт
    if (HAL_DMA_Abort(huart->hdmarx) != HAL_OK)
    {
      Error_Handler();
    }
    // Дожидаемся, пока DMA действительно станет READY
    while (huart->hdmarx->State != HAL_DMA_STATE_READY)
    { /* wait */
    }

    // «Выгружаем» лишний байт из DR: читаем ISR и затем RDR
    __IO uint32_t tmp;
    tmp = huart->Instance->ISR;
    tmp = huart->Instance->RDR;
    (void)tmp;

    // Сбрасываем флаг переполнения
    __HAL_UART_CLEAR_OREFLAG(huart);
  }

  // Перезапускаем приём по DMA
  if (HAL_UARTEx_ReceiveToIdle_DMA(huart, rxBuffer, RX_BUFFER_SIZE) != HAL_OK)
  {
    Error_Handler();
  }
  // По-прежнему отключаем Half-Transfer interrupt
  __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
}

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
    HAL_Delay(1000);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
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
