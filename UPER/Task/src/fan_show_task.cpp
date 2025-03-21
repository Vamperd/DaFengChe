/**
 *******************************************************************************
 * @file              :fan_show_task.cpp
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
#include "fan_show_task.hpp"
#include "main_task.hpp"
#include "rng.h"
/* Private macro -------------------------------------------------------------*/
#define FAN_COUNT 5
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
extern buf_t buf;
extern uint8_t buf_mode;
/* Private function prototypes -----------------------------------------------*/

void Fan_Show_Task(uint8_t buf_state, uint8_t buf_colour) {
  static uint8_t target_fan_ID;
  static uint32_t fan_tick = 0;
  static uint8_t none_idle_flag = 0;
   if (buf_mode == kBufMode_already) {
    if (fan_tick <= 750) {
      fan_tick++;
    } else {
      fan_tick = 0;
      All_Fan_Show_Turn_Off();
      none_idle_flag = 0;
      buf_mode = kBufMode_can;
    }
  } else if (none_idle_flag == 0) {
    if (fan_tick == 0) {
      target_fan_ID = Get_Rand_Idle_Fan(buf_state);

      fan_tick++;
      if (target_fan_ID == NONE_IDLE) {
        none_idle_flag = 1;
        fan_tick = 0;
      }
    } else if (buf.fan_feedback[target_fan_ID].fan_show_state ==
               FAN_ACTIVATED) {
      fan_tick = 0;
      buf.fan_target[target_fan_ID].fan_show_state = FAN_ACTIVATED;
    } else if (fan_tick >= 1250) {
      fan_tick = 0;
      All_Fan_Show_Turn_Off();
      none_idle_flag = 0;
    } else {
      fan_tick++;
    }
  }
  if(Illegal_hit_detection()){
    none_idle_flag = 0;
  }
}

void All_Fan_Show_Turn_Off(void) {
  for (uint8_t i = 0; i < 5;
       i++) // 2.5s后，将所有扇叶的状态重置为空闲，然后通过CAN下发到各个扇叶
  {
    buf.fan_target[i].fan_show_state = FAN_IDLE;
    buf.fan_target[i].ring_num = 0;
    buf.fan_feedback[i].fan_show_state = FAN_IDLE;
    buf.fan_feedback[i].ring_num = 0;
    // TxData[0] = (buf.fan_target[i].fan_show_state << 3) | (buf.buf_state <<
    // 1) | buf.colour; #TODO
    // 还不确定这么写两次can信息的收发会不会冲突，还需要测试
    // CAN_Send_Msg(&hcan2, TxData, MASTER_BASEADDR + i, 8);
  }
}

/**
 * @brief 随机获得空闲的扇叶ID，同时更新选中扇叶的目标状态
 * @retval 随机的空闲扇叶ID，如果所有扇叶都不是空闲状态则返回NONE_IDLE
 **/
uint8_t Get_Rand_Idle_Fan(uint8_t buf_state) {
  static uint8_t idle_fan_ID;
  // static uint8_t test_buf_flag = 0;
  uint8_t none_idle_flag = 0;

  // 暂时使用
  // buf.fan_target[1].fan_show_state = FAN_ACTIVATED;
  // buf.fan_target[1].ring_num  = 0;
  // buf.fan_feedback[1].fan_show_state = FAN_ACTIVATED;
  // buf.fan_feedback[1].ring_num = 0;

  for (uint8_t i = 0; i < FAN_COUNT; i++) {
    if (buf.fan_feedback[i].fan_show_state == FAN_IDLE) {
      none_idle_flag = 1;
      break;
    }
  }

  if (none_idle_flag != 1) {
    // test_buf_flag = 0;
    return NONE_IDLE;
  }

  // if(buf_state == SAFE_BUF)
  // {
  //     /*
  //     if(test_buf_flag == 0)
  //     {
  //         idle_fan_ID = 0;
  //         test_buf_flag = 1;
  //     }
  //     else
  //     {
  //         idle_fan_ID = (idle_fan_ID + 1) % 5;
  //     }
  //     */
  //     idle_fan_ID = 4;    // 保证一个扇叶常亮
  //     buf.fan_target[idle_fan_ID].fan_show_state = FAN_ACTIVATABLE;

  // }
  // else
  // {
  // test_buf_flag = 0;
  idle_fan_ID = (uint8_t)(HAL_RNG_GetRandomNumber(&hrng) % FAN_COUNT);

  while (buf.fan_feedback[idle_fan_ID].fan_show_state != FAN_IDLE) {
    idle_fan_ID = (uint8_t)(HAL_RNG_GetRandomNumber(&hrng) % FAN_COUNT);
  }
  buf.fan_target[idle_fan_ID].fan_show_state = FAN_ACTIVATABLE;
  // }
  return idle_fan_ID;
}
