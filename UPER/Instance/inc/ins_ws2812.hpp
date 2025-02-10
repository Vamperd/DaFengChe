/*
 * @Author: Vamper 3230104810@zju.edu.cn
 * @Date: 2025-01-16 22:25:44
 * @LastEditors: Vamper 3230104810@zju.edu.cn
 * @LastEditTime: 2025-01-17 20:30:25
 * @FilePath: \UPER\Instance\inc\ins_ws2812.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef  INS__WS2812__HPP
#define  INS__WS2812__HPP

#include "main.h"

#define ARROW_LED_NUM (256) //灯条灯珠数量
#define LED_NUMS (64) //灯珠数量


void ws2812_init(void);
void ws2812_set_RGB(uint8_t R, uint8_t G, uint8_t B, uint16_t num);
void ws2812_set_dark(void);
void R_Logo_Show(uint8_t colour);
void arrow_flow_refresh(void);
void fan_arrow_show(uint8_t colour/*, uint8_t fan_state*/);


#endif
