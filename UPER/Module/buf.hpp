/**
 *******************************************************************************
 * @file              :buf.hpp
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
#ifndef BUF_HPP_
#define BUF_HPP_
/* Includes ------------------------------------------------------------------*/
#include "can.h"
#include "dma.h"
#include "fan_show_task.hpp"
#include "ins_all.hpp"
#include "iwdg.h"
#include "motor_task.hpp"
#include "tim.h"
/* Exported macro ------------------------------------------------------------*/
enum BufMode_ {
  kBufMode_cannot = 1u,
  kBufMode_can = 2u,
  kBufMode_ing = 3u,
  kBufMode_already = 4u,
};
/* Exported types ------------------------------------------------------------*/
void BufInit(void);
void BufUpdate(void);
void BufRun(void);

void Buf_task(void);
void RcSetMode(void);

int Illegal_hit_detection(void);
int is_already(void);

void Runon_cannot(void);
void Runon_can(void);
void Runon_ing(void);
void Runon_already(void);

void vision_task(void);

void CommInit(void);
void setCommData(void);
void SendMsg(void);

#endif /*BUF_HPP_ */
