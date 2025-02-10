/**
 *******************************************************************************
 * @file              :fan_show_task.hpp
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
#ifndef FAN_SHOW_TASK_HPP_
#define FAN_SHOW_TASK_HPP_

/* Includes ------------------------------------------------------------------*/
#include "ins_all.hpp"
/* Exported macro ------------------------------------------------------------*/
#define NONE_IDLE   10
/* Exported types ------------------------------------------------------------*/

void Fan_Show_Task(uint8_t buf_state, uint8_t buf_colour);
void All_Fan_Show_Turn_Off(void);
uint8_t Get_Rand_Idle_Fan(uint8_t buf_state);


#endif /* FAN_SHOW_TASK_HPP_ */
