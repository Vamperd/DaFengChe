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
#include "can.h"
#include "dma.h"
#include "fan_show_task.hpp"
#include "ins_all.hpp"
#include "iwdg.h"
#include "motor_task.hpp"
#include "tim.h"
// TODO
/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
hw_motor::Motor *Roll_motor_ptr = CreateMotorRoll();
hw_rc::DT7 *rc_ptr = CreateRemoteControl();
hw_pid::MultiNodesPid *small_buf_pid_ptr = CreatePid_Small_Buf();
hw_pid::MultiNodesPid *Big_buf_pid_ptr = CreatePid_Big_Buf();
hw_comm::CanRxMgr *can_rx_mgr_ptr = CreateCan1RxMgr();
hw_comm::CanTxMgr *can_tx_mgr_ptr = CreateCan1TxMgr();
hw_comm::UartRxMgr *uart_rx_mgr_ptr = CreateRcRxMgr();
/* Private types -------------------------------------------------------------*/
buf_t buf(0, 0);
/* Private variables ---------------------------------------------------------*/

// float spd_ref_ = 0.0f;
// float spd_fdb_ = 0.0f;
// float ang_ref_ = 0.0f;
// float ang_fdb_ = 0.0f;
float cur_ref_ = 0.0f;
float cur_fdb_ = 0.0f;
uint32_t tick = 0;
int8_t dirction = 1;
uint8_t buf_mode = kBufMode_cannot;
uint8_t generate_flag = 0;

/* External variables --------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void RefreshIwdg(void) { HAL_IWDG_Refresh(&hiwdg); }
void MainTaskInit() {
  can_rx_mgr_ptr->addReceiver(Roll_motor_ptr);
  can_tx_mgr_ptr->addTransmitter(Roll_motor_ptr);
  can_rx_mgr_ptr->filterInit();
  can_rx_mgr_ptr->startReceive();
  HAL_CAN_Start(&hcan1);

  CanFilter_Init(&hcan2);
  HAL_CAN_Start(&hcan2);
  HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING);

  rc_ptr->registerUpdateCallback(RefreshIwdg);
  uart_rx_mgr_ptr->addReceiver(rc_ptr);
  uart_rx_mgr_ptr->startReceive();

  ws2812_init();
}

void RcSetMode() {
  switch (rc_ptr->rc_r_switch()) {
  case hw_rc::SwitchState::kUp:
    buf.colour = BLUE;
    break;
  case hw_rc::SwitchState::kMid: // #TODO 更改为停转长亮模式
    buf.colour = DARK;
    buf.reset();
    dirction = -1 * dirction;
    break;
  case hw_rc::SwitchState::kDown:
    buf.colour = RED;
    break;
  default:
    buf.reset();
    break;
  }
  switch (rc_ptr->rc_l_switch()) {
  case hw_rc::SwitchState::kUp:
    if (buf_mode == kBufMode_cannot) {
      buf_mode = kBufMode_can;
    }
    buf.buf_state = BIG_BUF;
    break;
  case hw_rc::SwitchState::kMid:
    buf.buf_state = SAFE_BUF;
    buf.reset();
    buf_mode = kBufMode_cannot;
    break;
  case hw_rc::SwitchState::kDown:
    if (buf_mode == kBufMode_cannot) {
      buf_mode = kBufMode_can;
    }
    buf.buf_state = SMALL_BUF;
    break;
  default:
    buf.reset();
    Motor_Stop();
    break;
  }
  if (rc_ptr->rc_wheel() > 0 &&
      rc_ptr->rc_wheel() <
          1.5) { // #TODO 由于对于遥控器拨轮值有问题，故正反转临界可能需要更改
    dirction = 1;
  } else if (rc_ptr->rc_wheel() > 1.5) {
    dirction = -1;
  }
}

void Runon_cannot(void) {
  Motor_Stop();
  buf.reset();
}

void Runon_can(void) {
  static uint32_t can_tick = 0;
  R_Logo_Show(buf.colour);
  if (can_tick >= 1500) {
    can_tick = 0;
    buf_mode = kBufMode_ing;
    generate_flag = 1;
  } else {
    can_tick++;
  }
}

void Runon_ing(void) {
  static uint32_t ing_tick = 0;
  Motor_Task(buf.buf_state, ing_tick, dirction);
  Fan_Show_Task(buf.buf_state, buf.colour);
  if (is_already()) {
    ing_tick = 0;
    buf_mode = kBufMode_already;
  }else if(Illegal_hit_detection()) {
    ing_tick = 0;
  } else {
    ing_tick++;
  }
}

void Runon_already(void) {
  Motor_Stop();
 // Motor_Task(buf.buf_state, ing_tick, dirction); #TODO 不确定全部激活后是否需要旋转
  Fan_Show_Task(buf.buf_state, buf.colour);
}

void MainTask() {
  // if (tick % 100 == 0) {
  R_Logo_Show(RED);
  // }
}

void setCommData() {
  HW_ASSERT(Roll_motor_ptr != nullptr, "pointer to roll Motor is nullptr",
            Roll_motor_ptr);
  if (!Roll_motor_ptr->isOffline() /*&& roll_mode != kRollMode_Off*/) {
    Roll_motor_ptr->setInput(cur_ref_);
  } else {
    Roll_motor_ptr->setInput(0.0f); // BLY
  }
}

void SendMsg() {
  can_tx_mgr_ptr->setTransmitterNeedToTransmit(Roll_motor_ptr);
  can_tx_mgr_ptr->startTransmit();
  // #TODO 增加需要板件通讯的发送数据
}

/**
 * @brief 检测是否有扇叶在非法状态下被击打
 * @retval 返回0表示有扇叶在非法状态下（已激活状态或者空闲状态）被击打
 **/
int Illegal_hit_detection(void) {
  for (uint8_t i = 0; i < 5; i++) {
    if (buf.fan_feedback[i].fan_show_state == FAN_ILLEGAL) {
      All_Fan_Show_Turn_Off();
      buf_mode = kBufMode_can;
      return 1;
    }
  }
  return 0;
}

int is_already(void) {
  uint8_t already_flag = 1;
  for (uint8_t i = 0; i < 5; i++) {
    if (buf.fan_feedback[i].fan_show_state != FAN_ACTIVATED) {
      already_flag = 0;
    }
  }
  return already_flag;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim == &htim6) {
    MainTask();
    HAL_IWDG_Refresh(&hiwdg);
    tick++;
  }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  HAL_TIM_PWM_Stop_DMA(&htim8, TIM_CHANNEL_3);
}
