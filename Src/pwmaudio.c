/*******************************************************************************
* File Name          : PwmAudio.c
* Author             : Taotao Yan
* Description        : This file provides all the PwmAudio functions.

* History:
*  12/20/2014 : PwmAudio V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "tim.h"
#include "pwmaudio.h"

/* Extern variables ----------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;

/* Public functions ----------------------------------------------------------*/
void StartPWMAudio(uint32_t freq, uint8_t ratio)
{
	/*To get PWM output clock at 24 KHz, the period (ARR)) is computed as follows:
	   ARR = (TIM2 counter clock / TIM2 output clock) - 1 = 665
	   
	TIM2 Channel1 duty cycle = (TIM2_CCR1/ TIM2_ARR + 1)* 100 = 50%

	Note:
	 SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f0xx.c file.
	 Each time the core clock (HCLK) changes, user had to update SystemCoreClock
	 variable value. Otherwise, any configuration based on this variable will be incorrect.
	 This variable is updated in three ways:
	  1) by calling CMSIS function SystemCoreClockUpdate()
	  2) by calling HAL API function HAL_RCC_GetSysClockFreq()
	  3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency*/

	/*Step1 Config PWM frequency*/
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_OC_InitTypeDef sConfigOC;

	htim2.Instance = TIM2;
	/* Compute the prescaler value to have TIM2 counter clock equal to 16000000 Hz */
	htim2.Init.Prescaler = (uint32_t)(SystemCoreClock / 24000000) - 1;
	;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = freq;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&htim2);
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);
	/*Step2 Config PWM duty cycle Common configuration for all channels */
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	sConfigOC.Pulse = (uint32_t)(freq * ratio / 100);
	HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
}

void StopPWMAudio(void)
{
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
