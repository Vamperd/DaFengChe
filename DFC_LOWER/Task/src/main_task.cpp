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
#include "ins_all.hpp"
#include "can.h"
#include "dma.h"
#include "tim.h"
/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t tick = 0;
uint8_t valuable_num_shoot_ring = 0;
UpCommand upcommand(FAN_IDLE,TEST_BUF,DARK,0x00);

/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/


void MainTask() {
  if(tick%100==0){
    get_shoot_ring();
    fan_show(upcommand.GetColour(),upcommand.GetFanShowState(),upcommand.GetBufState(),valuable_num_shoot_ring);
    }
  //# TEST
  // if(tick%80000==0){
  //   upcommand.SetFanShowState(FAN_ACTIVATABLE);
  //   CAN_Send_Msg(&hcan, &valuable_num_shoot_ring, SLV_BASEADDR + SLV_ID, 1);
  // }
}

void MainTaskInit() {
  CanFilter_Init(&hcan);
  HAL_CAN_Start(&hcan);
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING); 
  ws2812_init();
  // upcommand.SetBufState(BIG_BUF);
  // upcommand.SetColour(RED);
  // upcommand.SetFanShowState(FAN_ACTIVATABLE); //test
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  //20 000HZ
  if (htim == &htim4) {
    MainTask();
    tick++;
  }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
//   if(htim == &htim2) {
//    HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_3);
//    HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_2);
//    HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
// }
//   else if(htim == &htim3) {
//   HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_1);
//   HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_4);
// }
}