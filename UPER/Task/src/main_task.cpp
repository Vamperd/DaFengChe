/**
 *******************************************************************************
 * @file              :main_task.cpp
 * @brief             :
 * @history   :
 *  Version     Date            Author          Note
 *  V0.9.0      yyyy-mm-dd      <Vamper>        1。<note>
 *******************************************************************************
 * @attention :
 *******************************************************************************
 *  Copyright (c) 2025 Hello World Team,Zhejiang University.
 *  All Rights Reserved.
 *******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/

#include "main_task.hpp"
#include "buf.hpp"

/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

uint32_t tick = 0;

/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void MainTaskInit() {
  CommInit();
  BufInit();
  ws2812_init();
}

void MainTask() {
  BufUpdate();
  BufRun();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim == &htim6) {
    MainTask();
    // HAL_IWDG_Refresh(&hiwdg);//BLYTEST
    tick++;
  }
}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  // HAL_TIM_PWM_Stop_DMA(&htim8, TIM_CHANNEL_3);
}
