/**
  ******************************************************************************
  * File Name          : main.hpp
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define PC13_MCU_WAKUP_Pin GPIO_PIN_13
#define PC13_MCU_WAKUP_GPIO_Port GPIOC
#define PC13_MCU_WAKUP_EXTI_IRQn EXTI4_15_IRQn
#define PC0_BATT_ADC_Pin GPIO_PIN_0
#define PC0_BATT_ADC_GPIO_Port GPIOC
#define PC1_VIN_ADC_Pin GPIO_PIN_1
#define PC1_VIN_ADC_GPIO_Port GPIOC
#define PC2_PWR_EN_Pin GPIO_PIN_2
#define PC2_PWR_EN_GPIO_Port GPIOC
#define PA0_RESETN_Pin GPIO_PIN_0
#define PA0_RESETN_GPIO_Port GPIOA
#define PA2_MCU_TX2_Pin GPIO_PIN_2
#define PA2_MCU_TX2_GPIO_Port GPIOA
#define PA3_MCU_RX2_Pin GPIO_PIN_3
#define PA3_MCU_RX2_GPIO_Port GPIOA
#define PA4_FLASH_CS_N_Pin GPIO_PIN_4
#define PA4_FLASH_CS_N_GPIO_Port GPIOA
#define PA5_FLASH_SCK_Pin GPIO_PIN_5
#define PA5_FLASH_SCK_GPIO_Port GPIOA
#define PA6_FLASH_MISO_Pin GPIO_PIN_6
#define PA6_FLASH_MISO_GPIO_Port GPIOA
#define PA7_FLASH_MOSI_Pin GPIO_PIN_7
#define PA7_FLASH_MOSI_GPIO_Port GPIOA
#define PC4_MCU_TX3_Pin GPIO_PIN_4
#define PC4_MCU_TX3_GPIO_Port GPIOC
#define PC5_MCU_RX3_Pin GPIO_PIN_5
#define PC5_MCU_RX3_GPIO_Port GPIOC
#define PB0_BAT_ROUTE_EN_Pin GPIO_PIN_0
#define PB0_BAT_ROUTE_EN_GPIO_Port GPIOB
#define PB1_SW_LOAD_EN_Pin GPIO_PIN_1
#define PB1_SW_LOAD_EN_GPIO_Port GPIOB
#define PB2_PB2_RADIO_WAKE0_Pin GPIO_PIN_2
#define PB2_PB2_RADIO_WAKE0_GPIO_Port GPIOB
#define PB10_I2C2_SCL_Pin GPIO_PIN_10
#define PB10_I2C2_SCL_GPIO_Port GPIOB
#define PB11_I2C2_SDA_Pin GPIO_PIN_11
#define PB11_I2C2_SDA_GPIO_Port GPIOB
#define PB13_MCU_CTS3_Pin GPIO_PIN_13
#define PB13_MCU_CTS3_GPIO_Port GPIOB
#define PB14_MCU_RTS3_Pin GPIO_PIN_14
#define PB14_MCU_RTS3_GPIO_Port GPIOB
#define PC8_LED_G_Pin GPIO_PIN_8
#define PC8_LED_G_GPIO_Port GPIOC
#define PC9_LED_R_Pin GPIO_PIN_9
#define PC9_LED_R_GPIO_Port GPIOC
#define PA8_FLASH_WP_N_Pin GPIO_PIN_8
#define PA8_FLASH_WP_N_GPIO_Port GPIOA
#define PA9_MCU_TX1_Pin GPIO_PIN_9
#define PA9_MCU_TX1_GPIO_Port GPIOA
#define PA10_MCU_RX1_Pin GPIO_PIN_10
#define PA10_MCU_RX1_GPIO_Port GPIOA
#define PA15_MCU_RELAY_Pin GPIO_PIN_15
#define PA15_MCU_RELAY_GPIO_Port GPIOA
#define PC10_MCU_IGN_Pin GPIO_PIN_10
#define PC10_MCU_IGN_GPIO_Port GPIOC
#define PC11_MCU_OD_Pin GPIO_PIN_11
#define PC11_MCU_OD_GPIO_Port GPIOC
#define PC12_MCU_STARTER_Pin GPIO_PIN_12
#define PC12_MCU_STARTER_GPIO_Port GPIOC
#define PD2_MCU_IO_Pin GPIO_PIN_2
#define PD2_MCU_IO_GPIO_Port GPIOD
#define PB3_MCU_BUZZER_Pin GPIO_PIN_3
#define PB3_MCU_BUZZER_GPIO_Port GPIOB
#define PB4_G_INT1_Pin GPIO_PIN_4
#define PB4_G_INT1_GPIO_Port GPIOB
#define PB4_G_INT1_EXTI_IRQn EXTI4_15_IRQn
#define PB5_GPS_PWR_EN_Pin GPIO_PIN_5
#define PB5_GPS_PWR_EN_GPIO_Port GPIOB
#define PB6_I2C1_SCL_Pin GPIO_PIN_6
#define PB6_I2C1_SCL_GPIO_Port GPIOB
#define PB7_I2C1_SDA_Pin GPIO_PIN_7
#define PB7_I2C1_SDA_GPIO_Port GPIOB

#define LTE_PWR_PORT PC2_PWR_EN_GPIO_Port
#define LTE_PWR_PIN PC2_PWR_EN_Pin
#define LTE_RST_PORT PA0_RESETN_GPIO_Port
#define LTE_RST_PIN PA0_RESETN_Pin
#define MODEM_PWR_ON_PORT PB1_SW_LOAD_EN_GPIO_Port
#define MODEM_PWR_ON_PIN PB1_SW_LOAD_EN_Pin

#define TC30M_TEST_CONFIG_OFF (0)

#if TC30M_TEST_CONFIG_OFF
#define WEDGE_MINUTE_TO_SECOND_FACTOR (60)
#else
#define WEDGE_MINUTE_TO_SECOND_FACTOR (1) /* Just for test! */
#endif /* TC30M_TEST_CONFIG_OFF */

#define WEDGE_MSG_QUE_DATA_LEN_MAX (252)

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
 #define USE_FULL_ASSERT    1U 

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
