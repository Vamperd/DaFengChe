/**
 *******************************************************************************
 * @file              :motor_task.hpp
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MOTOR_TASK_HPP_
#define MOTOR_TASK_HPP_
/* Includes ------------------------------------------------------------------*/
#include "ins_all.hpp"
/* Exported macro ------------------------------------------------------------*/
#define SMALL_BUF_SPEED     (PI/3.0f)          //单位：rad/s
#define BIG_BUF_A_MIN       (0.780f)
#define BIG_BUF_A_MAX       (1.045f)
#define BIG_BUF_W_MIN       (1.884f)
#define BIG_BUF_W_MAX       (2.000f)
#define BIG_BUF_B_ERR       (2.090f)
/* Exported types ------------------------------------------------------------*/
/* Declare functions -*/
void Motor_Task(uint8_t buf_state, uint32_t tick_task,int8_t dirction);
void Moter_Big_Buf_Task(uint32_t tick_task,int8_t dirction);
void Moter_Small_Buf_Task(int8_t dirction);
void Motor_Update(void);
void Motor_Stop(void);
float Get_Rand_Num(float min, float max);


#endif /* MOTOR_TASK_HPP_ */
