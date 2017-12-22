/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
#include "stm32f0xx_it.h"

#include "uart_api.h"
#include "usrtimer.h"
#include "initialization.h"
#include "ublox_driver.h"

/* Private define ------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif
/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc;
extern RTC_HandleTypeDef hrtc;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern __IO uint8_t factorymodeindicatefalg;

/******************************************************************************/
/*            Cortex-M0 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

// /**
// * @brief This function handles Hard fault interrupt.
// */
// void HardFault_Handler(void)
// {
//   /* USER CODE BEGIN HardFault_IRQn 0 */

//   /* USER CODE END HardFault_IRQn 0 */
//   while (1)
//   {
//   }
//   /* USER CODE BEGIN HardFault_IRQn 1 */

//   /* USER CODE END HardFault_IRQn 1 */
// }

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  SoftwareTimerCounter();
  // HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles RTC Interrupt through EXTI lines 17, 19 and 20.
*/
void RTC_IRQHandler(void)
{
  /* USER CODE BEGIN RTC_IRQn 0 */

  /* USER CODE END RTC_IRQn 0 */
  HAL_RTC_AlarmIRQHandler(&hrtc);
  HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);
  /* USER CODE BEGIN RTC_IRQn 1 */

  /* USER CODE END RTC_IRQn 1 */
}

/**
* @brief This function handles EXTI line 4 to 15 interrupts.
*/
void EXTI4_15_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */

  /* USER CODE END EXTI4_15_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
  /* USER CODE BEGIN EXTI4_15_IRQn 1 */

  /* USER CODE END EXTI4_15_IRQn 1 */
}

/**
* @brief This function handles DMA1 channel 1 interrupt.
*/
void DMA1_Ch1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Ch1_IRQn 0 */

  /* USER CODE END DMA1_Ch1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc);
  /* USER CODE BEGIN DMA1_Ch1_IRQn 1 */

  /* USER CODE END DMA1_Ch1_IRQn 1 */
}

/**
* @brief This function handles USART1 global interrupt.
*/
static void UARTx_IRQ_Com_Handler(UART_HandleTypeDef *huart)
{
  /* UART parity error interrupt occurred ------------------------------------*/
  if (__HAL_UART_GET_FLAG(huart, UART_FLAG_PE) != RESET)
  {
    __HAL_UART_CLEAR_FEFLAG(huart);
    huart->ErrorCode |= HAL_UART_ERROR_PE;
  }
  /* UART frame error interrupt occurred -------------------------------------*/
  if (__HAL_UART_GET_FLAG(huart, UART_FLAG_FE) != RESET)
  {
    __HAL_UART_CLEAR_FEFLAG(huart);
    huart->ErrorCode |= HAL_UART_ERROR_FE;
  }
  /* UART noise error interrupt occurred -------------------------------------*/
  if (__HAL_UART_GET_FLAG(huart, UART_FLAG_NE) != RESET)
  {
    __HAL_UART_CLEAR_NEFLAG(huart);
    huart->ErrorCode |= HAL_UART_ERROR_NE;
  }
  /* UART Over-Run interrupt occurred ----------------------------------------*/
  if (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE) != RESET)
  {
    __HAL_UART_CLEAR_OREFLAG(huart);
  }
  /* UART wakeup from Stop mode interrupt occurred -------------------------------------*/
  /*if(__HAL_UART_GET_FLAG(huart, UART_IT_WUF) != RESET)
  { 
    __HAL_UART_GET_FLAG(huart, UART_CLEAR_WUF);
  }*/
  if (huart->ErrorCode != HAL_UART_ERROR_NONE)
  {
  }
}

/**
* @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
*/
void USART1_IRQHandler(void)
{

  UART_HandleTypeDef *huart = &huart1;
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  //HAL_UART_IRQHandler(&huart1);
  UARTx_IRQ_Com_Handler(&huart1);
  if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE) != RESET)
  {
    uint8_t RevData;
    /* Read one byte from the receive data register and send it back */
    RevData = (uint8_t)(huart->Instance->RDR & (uint8_t)0x00FF);

    if (0x1b == RevData)
    {
      factorymodeindicatefalg = FALSE;
      __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_RXNE);
      return;
    }
    huart->Instance->TDR = RevData;

    if (Uart1RxCount & UART_FIRST_END_CHAR) // Get 0x0D
    {
      if (RevData != UART1_END_CHAR_OA)
        Uart1RxCount = 0; // Restart
      else
      {
        Uart1RxCount |= UART_FINISHED_RECV; // Finished
      }
    }
    else
    {
      if (RevData == UART1_END_CHAR_OD)
        Uart1RxCount |= UART_FIRST_END_CHAR;
      else
      {
        Uart1RxBuffer[Uart1RxCount & UART_BUF_MAX_LENGTH] = RevData;
        Uart1RxCount++;
        if (Uart1RxCount >= UART_BUF_MAX_LENGTH)
          Uart1RxCount = 0; // Error
      }
    }

    /* Clear RXNE interrupt flag */
    //__HAL_UART_FLUSH_DRREGISTER(huart);
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_RXNE);
  }
}

/**
* @brief This function handles USART2 global interrupt / USART2 wake-up interrupt through EXTI line 26.
*/
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
* @brief This function handles USART3 to USART8 global interrupts / USART3 wake-up interrupt through EXTI line 28.
*/
void USART3_8_IRQHandler(void)
{
  uint8_t tmp = 0;
  UART_HandleTypeDef *huart = &huart3;
  /* USER CODE BEGIN USART6_IRQn 0 */

  /* USER CODE END USART6_IRQn 0 */
  //HAL_UART_IRQHandler(&huart3);
  UARTx_IRQ_Com_Handler(&huart3);
  if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE) != RESET)
  {
    /* Read one byte from the receive data register and send it back */
    tmp = (uint8_t)(huart->Instance->RDR & (uint8_t)0x00FF);

    Uart3RxBuffer[Uart3RxCount] = tmp;
    Uart3RxCount++;
    if (Uart3RxCount >= (UART3_RX_BUFFER_SIZE - 2))
    {
      Uart3RxCount = 0;
    }

    SoftwareTimerReset(&UART3RevCDMATimer, CheckUART3RevCDMATimerCallback, CHECK_UART3RevCDMATimer_STAT_TIMEOUT);
    SoftwareTimerStart(&UART3RevCDMATimer);
    
    /* Clear RXNE interrupt flag */
    //__HAL_UART_FLUSH_DRREGISTER(huart);
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_RXNE);
  }
  /* USER CODE BEGIN USART6_IRQn 1 */

  /* USER CODE END USART6_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
