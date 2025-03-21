/*
 * @Author: Vamper 3230104810@zju.edu.cn
 * @Date: 2025-01-16 16:53:48
 * @LastEditors: Vamper 3230104810@zju.edu.cn
 * @LastEditTime: 2025-01-17 22:54:09
 * @FilePath: \UPER\Task\inc\main_task.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/** 
 *******************************************************************************
 * @file      : main_task.hpp
 * @brief     : 
 * @history   :
 *  Version     Date            Author          Note
 *  V0.9.0      yyyy-mm-dd      <Vamper>        1. <note>
 *******************************************************************************
 * @attention :
 *******************************************************************************
 *  Copyright (c) 2024 Hello World Team, Zhejiang University.
 *  All Rights Reserved.
 *******************************************************************************
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_TASK_HPP_
#define MAIN_TASK_HPP_

/* Includes ------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
enum BufMode_{
  kBufMode_cannot=1u,
  kBufMode_can=2u,
  kBufMode_ing=3u,
  kBufMode_already=4u,
};
/* Exported variables --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
void MainTaskInit(void);
void MainTask(void);
int Illegal_hit_detection(void);
int is_already(void);
void Runon_cannot(void);
void Runon_can(void);
void Runon_ing(void);
void Runon_already(void);
void setCommData(void);
void SendMsg(void);

#ifdef __cplusplus
}
#endif
#endif /* MAIN_TASK_HPP_ */
