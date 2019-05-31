/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 ** This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * COPYRIGHT(c) 2018 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *notice, this list of conditions and the following disclaimer in the
 *documentation and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "end.h"
#include "onenet.h"
#include "stdio.h"
#include "string.h"
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

/* USER CODE BEGIN PV */
int tcounter = 0;
extern uint32_t TimeTick;
DevState_e DevState = Dev_Init;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void Usart_Send1(uint8_t *buff, uint8_t size) {
  HAL_UART_Transmit(Usart_Z.huart, buff, size, 10);
}
static void NextRx1(void) {
  memset(Usart_Z.buff, 0, sizeof(Usart_Z.buff));
  Usart_Z.len = 0;
  __HAL_UART_ENABLE_IT(Usart_Z.huart, UART_IT_RXNE);
  __HAL_UART_CLEAR_IDLEFLAG(Usart_Z.huart);
  Usart_Z.State = RxReady;
}

static void Usart_Send2(uint8_t *buff, uint8_t size) {
  HAL_UART_Transmit(Usart_E.huart, buff, size, 10);
}
static void NextRx2(void) {
  memset(Usart_E.buff, 0, sizeof(Usart_E.buff));
  Usart_E.len = 0;
  __HAL_UART_ENABLE_IT(Usart_E.huart, UART_IT_RXNE);
  __HAL_UART_CLEAR_IDLEFLAG(Usart_E.huart);
  Usart_E.State = RxReady;
}
static void Board_Init() {
  Head.IsNet = false;
  Usart_Z.huart = &huart1;
  Usart_Z.Send = Usart_Send1;
  Usart_Z.Rx = NextRx1;
  Usart_Z.Rx();
  Usart_E.huart = &huart2;
  Usart_E.Send = Usart_Send2;
  Usart_E.Rx = NextRx2;
  Usart_E.Rx();
  Head.Process = EndMsg;
  HAL_TIM_Base_Start_IT(&htim2);
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  Board_Init();
  ETH_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */
    ETHDev.Process();
    Head.Process();
    switch (DevState) {
      case Dev_Init:
        DevState = DevJoin;
        break;
      case DevJoin:
        if (Head.IsNet == false && (TimeTick % 5000) == 0) {
          ETHDev.Connect();
					DevState = DevRun;
        }
        break;
      case DevRun:
        if (Head.IsNet == false) {
          DevState = DevJoin;
          break;
        } else if (tcounter >= 6 && Head.IsNet == true) {
          ETHDev.SendValue("Online_Device", Head.end_num);
//          for (int i = 0; i < Head.end_num; i++) {
            // SendStringToOnenet(Head.save_end[i]);
//          }
        }
        break;
      default:
				DevState = Dev_Init;
        break;
    }
    // if (state2 == 0x03)
    // {
    //     IsOnenetCmd(rxbuf2);
    //     state2 = 0x01;
    //     memset(rxbuf2, 0, 50);
    //     __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
    // }
    // if (tcounter >= 6 && Head.online_state == 1)
    // {
    //     SendValueToOnenet("Online_Device", Head.end_num);
    //     for (int i = 0; i < Head.end_num; i++)
    //     {
    //         SendStringToOnenet(Head.save_end[i]);
    //     }
    //     tcounter = 0;
    // }
    // if (state1 == 0x03)
    // {
    //     EndMsg((const uint8_t *)rxbuf1);
    //     state1 = 1;
    //     __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
    // }
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /**Initializes the CPU, AHB and APB busses clocks
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) { tcounter++; }
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
   number, tex: printf("Wrong parameters value: file %s on line %d\r\n", file,
   line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
