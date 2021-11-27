/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
extern bool rtd_io;
extern  bool precharge_io;
extern bool reset_io;
extern bool rtd_start;
extern bool clear_fault_io;
extern bool inverter_connect_R;
extern bool inverter_connect_L;
extern bool record;
extern uint16_t start_record_time;
extern uint16_t record_time;
extern uint16_t record_cnt;
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|pedals_LED_Pin|readyToDrive_LED_Pin|fault_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(precharge_LED_GPIO_Port, precharge_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CAN_fault_LED_GPIO_Port, CAN_fault_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PEPin PEPin */
  GPIO_InitStruct.Pin = readyToDrive_SW_Pin|clear_fault_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = logger_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(logger_SW_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = precharge_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(precharge_SW_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 PAPin PAPin PAPin */
  GPIO_InitStruct.Pin = GPIO_PIN_1|pedals_LED_Pin|readyToDrive_LED_Pin|fault_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = precharge_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(precharge_LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = CAN_fault_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CAN_fault_LED_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */
	if (GPIO_Pin==readyToDrive_SW_Pin){
		//ready to drive trigger
		//if((HAL_GPIO_ReadPin(readyToDrive_SW_GPIO_Port,readyToDrive_SW_Pin)==GPIO_PIN_RESET)&&reset_io==1&&precharge_io==1){
		if(HAL_GPIO_ReadPin(readyToDrive_SW_GPIO_Port,readyToDrive_SW_Pin)==GPIO_PIN_RESET && precharge_io && inverter_connect_R && inverter_connect_L){
			rtd_start=1;
		}else{
			rtd_start=0;
		}
	}else if(GPIO_Pin==precharge_SW_Pin){
		//precharge pin trigger
		if(HAL_GPIO_ReadPin(precharge_SW_GPIO_Port,precharge_SW_Pin)==GPIO_PIN_SET){
			precharge_io=1;
			HAL_GPIO_WritePin(precharge_LED_GPIO_Port,precharge_LED_Pin,GPIO_PIN_SET);
			//HAL_GPIO_WritePin(CAN_fault_LED_GPIO_Port,CAN_fault_LED_Pin,GPIO_PIN_SET);
		}else{
			precharge_io=0;
			rtd_io=0;
			rtd_start=0;
			HAL_GPIO_WritePin(readyToDrive_LED_GPIO_Port,readyToDrive_LED_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(precharge_LED_GPIO_Port,precharge_LED_Pin,GPIO_PIN_RESET);
			//HAL_GPIO_WritePin(CAN_fault_LED_GPIO_Port,CAN_fault_LED_Pin,GPIO_PIN_RESET);
		}
	}else if(GPIO_Pin==clear_fault_SW_Pin){
		//inverter clear fault PIN
		if(HAL_GPIO_ReadPin(clear_fault_SW_GPIO_Port,clear_fault_SW_Pin)==GPIO_PIN_RESET){
			clear_fault_io=1;
		}
	}else if(GPIO_Pin == logger_SW_Pin){
		if(HAL_GPIO_ReadPin(logger_SW_GPIO_Port,logger_SW_Pin)==GPIO_PIN_RESET){
			record = 1;
			start_record_time = HAL_GetTick();
			++record_cnt;
		}else{
			record = 0;
		}
	}
}
/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
