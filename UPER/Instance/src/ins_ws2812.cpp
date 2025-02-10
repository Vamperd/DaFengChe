/*
 * @Author: Vamper 3230104810@zju.edu.cn
 * @Date: 2025-01-16 22:25:54
 * @LastEditors: Vamper 3230104810@zju.edu.cn
 * @LastEditTime: 2025-01-17 22:51:36
 * @FilePath: \UPER\Instance\src\ins_ws2812.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "ins_all.hpp"
#include "dma.h"
#include "tim.h"

/* WS2812B f=800k, T=1.25us */
#define ONE_PULSE (143)   // 1 码 (2/3*T)
#define ZERO_PULSE (67)   // 0 码 (1/3*T)
#define RESET_PULSE (500) // 低电平复位信号50us
#define LED_DATA_LEN (24) // led 颜色数据长度, 一个灯珠需要24bits
#define WS2812_DATA_LEN                                                        \
  (RESET_PULSE + ARROW_LED_NUM * LED_DATA_LEN) // ws2812灯条需要的总数组长度
#define WS2812_DATA_LEN_1                                                        \
  (RESET_PULSE + LED_NUMS * LED_DATA_LEN) // ws2812灯条需要的总数组长度


// 箭头流动效果下灯珠亮灭状态储存，每5行为一个单位进行存储，显示状态完全相同
// 下列数组等效显示效果
//  1 1 0 0 0 0 1 1
//  1 1 1 0 0 1 1 1
//  0 1 1 1 1 1 1 0
//  0 0 1 1 1 1 0 0
//  0 0 0 1 1 0 0 0
// static uint8_t single_arrow_flow[5] = {0x18, 0x3C, 0x7E, 0xE7, 0xC3};
// static uint8_t arrow_flow[32] = {0};

// static uint16_t ARROW_buffer[WS2812_DATA_LEN] = {0};
static uint16_t RGB_buffer[WS2812_DATA_LEN_1] = {0};

uint16_t colour_rgb[3][3] = {
    {0, 0, 20}, // 蓝色的RGB
    {20, 0, 0}, // 红色的RGB
    {0, 0, 0}   // 不亮的RGB
};
/* ------------------------------ Function Declaration ------------------------------ */
static void ws2812_refresh(void);

/* ------------------------------ Function Definition ------------------------------ */
void ws2812_init(void) { ws2812_set_dark(); }

void ws2812_set_RGB(uint8_t R, uint8_t G, uint8_t B, uint16_t num) {
  if (num >LED_NUMS /*ARROW_LED_NUM*/)
    return;

  uint16_t *p = (RGB_buffer + RESET_PULSE) + (num * LED_DATA_LEN);

  for (uint16_t i = 0; i < 8; i++) {
    p[i] = (G << i) & (0x80) ? ONE_PULSE : ZERO_PULSE;
    p[i + 8] = (R << i) & (0x80) ? ONE_PULSE : ZERO_PULSE;
    p[i + 16] = (B << i) & (0x80) ? ONE_PULSE : ZERO_PULSE;
  }
}

void ws2812_set_dark(void) {
  for (uint16_t i = 0; i < LED_NUMS; i++) {
    ws2812_set_RGB(0x00, 0x00, 0x00, i);
  }
  ws2812_refresh();
}

void R_Logo_Show(uint8_t colour) {
  uint8_t r, g, b;
  r = colour_rgb[colour][0];
  g = colour_rgb[colour][1];
  b = colour_rgb[colour][2];

  for (int i = 0; i < LED_NUMS; i++) {
    ws2812_set_RGB(r, g, b, i);
  }
  // ws2812_set_RGB(r, g, b, 0);
  ws2812_refresh();
}

void ws2812_test(void) {
  int num = 10;
  for (int i = 0; i < num; i++) {
    ws2812_set_RGB(255, 6.9, 6.9, i);
    // ws2812_set_RGB(0x20,0x7f,0x00,i);
  }
  ws2812_refresh();
}

// void arrow_flow_refresh(void)
// {
//     // 将单个箭头的形状映射到传输数据的顺序
//     uint8_t temp_single_row = 0;
//     for(uint8_t m = 0; m < 32; m++)
//     {
//         arrow_flow[m] = single_arrow_flow[temp_single_row];
//         temp_single_row++;
//         if(temp_single_row > 4)
//         {
//             temp_single_row = 0;
//         }
//     }
//     // 更新下次显示的单位箭头形状
    
    
//     uint8_t temp_arrow_row = single_arrow_flow[0];
//     uint8_t i;
//     for (i = 0; i < 4; i++)
//     {
//         single_arrow_flow[i] = single_arrow_flow[i + 1];
//     }
//     single_arrow_flow[i] = temp_arrow_row;
// }

// void fan_arrow_show(uint8_t colour/*, uint8_t fan_state*/)
// {
//     uint8_t r, g, b;
//     // if (fan_state == FAN_IDLE) // 空闲状态下所有灯珠不亮
//     // {
//     //     colour = DARK;
//     // }
//     r = colour_rgb[colour][0];
//     g = colour_rgb[colour][1];
//     b = colour_rgb[colour][2];

//     // if (fan_state == FAN_IDLE || fan_state == FAN_ACTIVATED)
//     // {
//     //     for (uint16_t i = 0; i < ARROW_LED_NUM; i++)
//     //     {
//     //         ws2812_set_RGB(r, g, b, i, ARROW_SECTION);
//     //     }
//     // }
//     // else if (fan_state == FAN_ACTIVATABLE)
//     // {
//         arrow_flow_refresh();

//         for(uint8_t m = 0; m < 32; m++)
//         {
//             for(uint8_t n = 0; n < 8; n++)
//             {
//                 uint16_t temp_num = m * 8 + n;
//                 ((arrow_flow[m] >> (7 - n)) & 0x01) == 0 ? ws2812_set_RGB(0x00, 0x00, 0x00, temp_num/*, ARROW_SECTION*/) : ws2812_set_RGB(r, g, b, temp_num/*, ARROW_SECTION*/);
//             }
//         }
//     // }

//     ws2812_refresh();
// }

static void ws2812_refresh(void) {
  HAL_TIM_PWM_Start_DMA(&htim8, TIM_CHANNEL_3, (uint32_t *)RGB_buffer,
                        WS2812_DATA_LEN_1);
}
