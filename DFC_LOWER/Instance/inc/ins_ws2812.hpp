/**
 *******************************************************************************
 * @file              :ins_ws2812.hpp
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
#ifndef INS__WS2812__HPP
#define INS__WS2812__HPP
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported macro ------------------------------------------------------------*/
#define LED_NUMS (10) //灯珠数量

#define FRAME_DOWM_LED_NUM  (90)      //TODO 扇叶上边框灯珠数量 
#define FRAME_UP_LED_NUM    (90)      //TODO 扇叶下边框灯珠数量 
#define SCREEN_LED_NUM      (292)      //TODO 扇叶屏幕灯珠数量
#define ARROW_LED_NUM       (256)      //TODO 扇叶箭头灯珠数量
#define SURROUND_LED_NUM    (140)      //TODO 扇叶周边灯带灯珠数量 

#define FRAME_DOWN_SECTION          (0)
#define FRAME_UP_SECTION            (1)
#define SCREEN_SECTION              (2)
#define ARROW_SECTION               (3)
#define SURROUND_SECTION            (4)

/* Exported types ------------------------------------------------------------*/
void ws2812_init(void);
void ws2812_set_RGB(uint8_t R, uint8_t G, uint8_t B, uint16_t num);
void ws2812_set_dark(void);

void fan_frame_show(uint8_t colour, uint8_t fan_state);
void fan_arrow_show(uint8_t colour/*, uint8_t fan_state*/);
void fan_surround_show(uint8_t colour, uint8_t fan_state);
void fan_screen_show(uint8_t colour, uint8_t fan_state, uint8_t buf_state, uint8_t ring_num);
void fan_show(uint8_t colour, uint8_t fan_state, uint8_t buf_state, uint8_t ring_num);
void arrow_flow_refresh(void);



void R_Logo_Show(uint8_t colour);

#endif /*INS__WS2812__HPP_ */



