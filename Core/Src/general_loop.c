#include "general_loop.h"
#include "main.h"
#include "adc.h"
#include "stm32h5xx_hal_adc.h"
#include "stm32h5xx_hal_tim.h"
#include "stm32h5xx_hal_uart.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

volatile uint16_t adcValue1 = 0;
volatile uint16_t adcValue2 = 0;
volatile char uartRxBuffer[4];

void updatePwm(uint8_t index) {
    uint32_t pulse = 1000;

    switch(index) {
        case 0:
            pulse = 1000 * adcValue1 / 4095; // Scale ADC value to PWM range (0-1000)

            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pulse);
            break;
        case 1:
            pulse = 1000 * adcValue2 / 4095; // Scale ADC value to PWM range (0-1000)
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pulse);
            break;
        // case 2:
        //     pulse = ((adcValue1 + adcValue2) / 2 * 1000) / 4095; // Average and scale
        //     __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, pulse);
        //     break;
        default:
            break;
    }
    //HAL_TIM_pwm
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
  static uint8_t channelIndex = 0;
  
  if (hadc->Instance == ADC1) {
    if(channelIndex == 0) {
      adcValue1 = HAL_ADC_GetValue(hadc);
    }
    else{
      adcValue2 = HAL_ADC_GetValue(hadc);
    }
    updatePwm(channelIndex);
    channelIndex++;
    if(channelIndex >= 2) channelIndex = 0;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART3) {
        uint16_t ch3 = (uartRxBuffer[2] << 8) + uartRxBuffer[3];
        // Echo received data back
        // HAL_UART_Transmit(huart, (uint8_t*)uartRxBuffer, sizeof(uartRxBuffer), HAL_MAX_DELAY);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, ch3);
        // ch3 += 25;
        // if(ch3 > 1000) ch3 = 0;
        // Restart UART reception
        HAL_UART_Receive_IT(huart, (uint8_t*)uartRxBuffer, sizeof(uartRxBuffer));
    }
}

void general_loop(void)
{
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // Start PWM on TIM1 Channel 1
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); // Start PWM on TIM1 Channel 1
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); // Start PWM on TIM1 Channel 1
  HAL_TIM_Base_Start(&htim1);
  HAL_UART_Receive_IT(&huart3, (uint8_t*)uartRxBuffer, sizeof(uartRxBuffer));
  HAL_ADC_Start_IT(&hadc1);
  //HAL_
  char mess[30];
  HAL_ADC_Start(&hadc1);
  while (1)
  {

      snprintf(mess, sizeof(mess), "\x1B[2K\rADC1: %lu, ADC2: %lu", adcValue1, adcValue2);
      HAL_UART_Transmit(&huart3, (uint8_t*)mess, strlen(mess), HAL_MAX_DELAY);

      //HAL_UART_Transmit(&huart3, (uint8_t *)mess, 20, HAL_MAX_DELAY);
      //HAL_Delay(500);
      HAL_Delay(250);

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}