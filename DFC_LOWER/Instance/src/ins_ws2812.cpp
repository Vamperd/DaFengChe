/**
 *******************************************************************************
 * @file              :ins_ws2812.cpp
 * @brief             :
 * @history   :
 *  Version     Date            Author          Note
 *  V0.9.0      2025-1-19      <Vamper>        1。<note>
 *******************************************************************************
 * @attention :
 *******************************************************************************
 *  Copyright (c) 2025 Hello World Team,Zhejiang University.
 *  All Rights Reserved.
 *******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/

#include "dma.h"
#include "ins_all.hpp"
#include "main.h"
#include "tim.h"
/* Private macro -------------------------------------------------------------*/

/* WS2812B f=800k, T=1.25us */
#define ONE_PULSE (61)    // 1 码 (2/3*T)
#define ZERO_PULSE (29)   // 0 码 (1/3*T)
#define RESET_PULSE (450) // 低电平复位信号50us
#define LED_DATA_LEN (24) // led 颜色数据长度, 一个灯珠需要24bits

#define WS2812_FRAME_DOWN_DATA_LEN                                             \
  (RESET_PULSE +                                                               \
   FRAME_DOWM_LED_NUM * LED_DATA_LEN) // 扇叶上边框灯珠驱动信号长度
#define WS2812_FRAME_UP_DATA_LEN                                               \
  (RESET_PULSE + FRAME_UP_LED_NUM * LED_DATA_LEN) // 扇叶下边框灯珠驱动信号长度
#define WS2812_SCREEN_DATA_LEN                                                 \
  (RESET_PULSE + SCREEN_LED_NUM * LED_DATA_LEN) // 扇叶屏幕灯珠驱动信号长度
#define WS2812_ARROW_DATA_LEN                                                  \
  (RESET_PULSE + ARROW_LED_NUM * LED_DATA_LEN) // 扇叶箭头灯珠驱动信号长度
#define WS2812_SURROUND_DATA_LEN                                               \
  (RESET_PULSE + SURROUND_LED_NUM * LED_DATA_LEN) // 扇叶周边灯珠驱动信号长度

/* Private constants ---------------------------------------------------------*/

// 箭头流动效果下灯珠亮灭状态储存
//  1 1 0 0 0 0 1 1
//  1 1 1 0 0 1 1 1
//  0 1 1 1 1 1 1 0
//  0 0 1 1 1 1 0 0
//  0 0 0 1 1 0 0 0
// 通过将32行反复填充并更新single实现流动效果
// TODO 单独开一路定时或TICK实现流动
static uint8_t single_arrow_flow[5] = {0x18, 0x3C, 0x7E, 0xE7, 0xC3};
static uint8_t arrow_flow[32] = {0};

// 扇叶各部分数组
static uint16_t frame_down_buffer[WS2812_FRAME_DOWN_DATA_LEN] = {0};
static uint16_t frame_up_buffer[WS2812_FRAME_UP_DATA_LEN] = {0};
static uint16_t screen_buffer[WS2812_SCREEN_DATA_LEN] = {0};
static uint16_t arrow_buffer[WS2812_ARROW_DATA_LEN] = {0};
static uint16_t surround_buffer[WS2812_SURROUND_DATA_LEN] = {0};

// 各个部分灯珠的数量
static uint16_t ws2812_num[5] = {FRAME_DOWM_LED_NUM, FRAME_UP_LED_NUM,
                                 SCREEN_LED_NUM, ARROW_LED_NUM,
                                 SURROUND_LED_NUM};

// 扇叶各环数亮到的灯珠的索引
static int16_t ring_led_index[11] = {-1,      4 - 1,   12 - 1,  28 - 1,
                                     52 - 1,  84 - 1,  116 - 1, 156 - 1,
                                     196 - 1, 244 - 1, 292 - 1};

// #TODO 十字交叉灯珠索引
static uint16_t cross_led_index[40] = {
    28,  34,  40,  46,  84,  91,  92,  99,  100, 107, 108, 115, 116, 117,
    126, 127, 136, 137, 146, 147, 156, 165, 166, 175, 176, 185, 186, 195,
    244, 254, 255, 256, 266, 267, 268, 278, 279, 280, 290, 291};

/* Private variables ---------------------------------------------------------*/
uint16_t colour_rgb[3][3] = {
    {0, 0, 50}, // 蓝色的RGB
    {50, 0, 0}, // 红色的RGB
    {0, 0, 0}   // 不亮的RGB
};

extern uint32_t tick;

/* Private function declarations
 * -----------------------------------------------*/
static void ws2812_refresh(void);
static void ws2812_fan_refresh(uint8_t led_section);

/* Private function prototypes -----------------------------------------------*/
void ws2812_init(void) { ws2812_set_dark(); }
/**
 * @brief 设置某个灯珠颜色RGB
 * @param uint8_t R ,G , B: RGB色彩格式 红,绿,蓝通道数据，一般只设定红蓝
 * @param uint16_t num, 指定设置颜色的灯珠位号
 * @param led_section 显示的部分
 **/
void ws2812_set_RGB(uint8_t R, uint8_t G, uint8_t B, uint16_t num,
                    uint8_t led_section) {
  if (num > ws2812_num[led_section])
    return;
  uint16_t *p;
  switch (led_section) {
  case FRAME_DOWN_SECTION:
    p = (frame_down_buffer + RESET_PULSE) + (num * LED_DATA_LEN);
    break;
  case FRAME_UP_SECTION:
    p = (frame_up_buffer + RESET_PULSE) + (num * LED_DATA_LEN);
    break;
  case SCREEN_SECTION:
    p = (screen_buffer + RESET_PULSE) + (num * LED_DATA_LEN);
    break;
  case ARROW_SECTION:
    p = (arrow_buffer + RESET_PULSE) + (num * LED_DATA_LEN);
    break;
  case SURROUND_SECTION:
    p = (surround_buffer + RESET_PULSE) + (num * LED_DATA_LEN);
    break;
  default:
    break;
  }

  for (uint16_t i = 0; i < 8; i++) {
    p[i] = (G << i) & (0x80) ? ONE_PULSE : ZERO_PULSE;
    p[i + 8] = (R << i) & (0x80) ? ONE_PULSE : ZERO_PULSE;
    p[i + 16] = (B << i) & (0x80) ? ONE_PULSE : ZERO_PULSE;
  }
}

/**
 * @brief 灭灯
 **/
void ws2812_set_dark(void) {
  for (uint8_t i = 0; i < 5; i++) {
    for (uint16_t j = 0; j < ws2812_num[i]; j++) {
      ws2812_set_RGB(0x00, 0x00, 0x00, j, i);
    }
  }

  for (uint8_t i = 0; i < 5; i++) {
    ws2812_fan_refresh(i);
  }
}

/**
 * @brief 理论上只有upper需要，这里用于test
 * @param colour 颜色
 **/
// #TODO 尚未测试
void R_Logo_Show(uint8_t colour) {
  uint8_t r, g, b;
  r = colour_rgb[colour][0];
  g = colour_rgb[colour][1];
  b = colour_rgb[colour][2];

  // for (int i = 0; i < ARROW_LED_NUM; i++) {
  //   ws2812_set_RGB(r, g, b, i);
  // }
  ws2812_set_RGB(r, g, b, 0);
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

/**
 * @brief  刷新上下边框灯珠的显示状态
 * @param  colour 显示的颜色——红、蓝、灭
 * @param  fan_state 扇叶状态
 * @retval none
 * @note
 *空闲状态下所有灯珠不亮，可激活和已激活状态下所有灯珠常亮；上下边框的显示状态完全一致
 **/
void fan_frame_show(uint8_t colour, uint8_t fan_state) {
  uint16_t r, g, b;
  if (fan_state == FAN_IDLE || fan_state == FAN_ILLEGAL) {
    colour = DARK;
  }
  r = colour_rgb[colour][0];
  g = colour_rgb[colour][1];
  b = colour_rgb[colour][2];

  for (uint8_t i = 0; i < FRAME_DOWM_LED_NUM; i++) {
    ws2812_set_RGB(r, g, b, i, FRAME_DOWN_SECTION);
  }
  for (uint8_t i = 0; i < FRAME_UP_LED_NUM; i++) {
    ws2812_set_RGB(r, g, b, i, FRAME_UP_SECTION);
  }

  ws2812_fan_refresh(FRAME_DOWN_SECTION);
  ws2812_fan_refresh(FRAME_UP_SECTION);
}

void arrow_flow_refresh(void) {
  // 将单个箭头的形状映射到传输数据的顺序
  uint8_t temp_single_row = 0;
  for (uint8_t m = 0; m < 32; m++) {
    arrow_flow[m] = single_arrow_flow[temp_single_row];
    temp_single_row++;
    if (temp_single_row > 4) {
      temp_single_row = 0;
    }
  }
  // 更新下次显示的单位箭头形状

  uint8_t temp_arrow_row = single_arrow_flow[0];
  uint8_t i;
  for (i = 0; i < 4; i++) {
    single_arrow_flow[i] = single_arrow_flow[i + 1];
  }
  single_arrow_flow[i] = temp_arrow_row;
}

void fan_arrow_show(uint8_t colour, uint8_t fan_state) {
  uint8_t r, g, b;
  if (fan_state == FAN_IDLE) // 空闲状态下所有灯珠不亮
  {
    colour = DARK;
  }
  r = colour_rgb[colour][0];
  g = colour_rgb[colour][1];
  b = colour_rgb[colour][2];

  if (fan_state == FAN_IDLE || fan_state == FAN_ACTIVATED) {
    for (uint16_t i = 0; i < ARROW_LED_NUM; i++) {
      ws2812_set_RGB(r, g, b, i, ARROW_SECTION);
    }
  } else if (fan_state == FAN_ACTIVATABLE) {
    if (tick % 5000 == 0) {
      arrow_flow_refresh();
    }
    for (uint8_t m = 0; m < 32; m++) {
      for (uint8_t n = 0; n < 8; n++) {
        uint16_t temp_num = m * 8 + n;
        ((arrow_flow[m] >> (7 - n)) & 0x01) == 0
            ? ws2812_set_RGB(0x00, 0x00, 0x00, temp_num, ARROW_SECTION)
            : ws2812_set_RGB(r, g, b, temp_num, ARROW_SECTION);
      }
    }
  }

  ws2812_fan_refresh(ARROW_SECTION);
}

/**
 * @brief  刷新周边灯带的显示状态
 * @param  colour 显示的颜色——红、蓝、灭
 * @param  fan_state 扇叶状态
 * @retval none
 * @note   已激活状态下所有灯珠常亮，空闲状态和可激活状态下所有灯珠不亮
 **/
void fan_surround_show(uint8_t colour, uint8_t fan_state) {
  uint8_t r, g, b;
  if (fan_state == FAN_IDLE ||
      fan_state == FAN_ACTIVATABLE) // 只有在已激活的情况下才点亮
  {
    colour = DARK;
  }
  r = colour_rgb[colour][0];
  g = colour_rgb[colour][1];
  b = colour_rgb[colour][2];

  for (uint16_t i = 0; i < SURROUND_LED_NUM; i++) {
    ws2812_set_RGB(r, g, b, i, SURROUND_SECTION);
  }

  ws2812_fan_refresh(SURROUND_SECTION);
}

// #TODO 可激活状态的具体亮起区域需要调试
/**
 * @brief  刷新屏幕灯珠的显示状态
 * @param  colour 显示的颜色——红、蓝、灭
 * @param  fan_state 扇叶状态
 * @param  buf_state 能量机关类型——大能量机关、小能量机关
 * @param  ring_num  击中环数
 * @retval none
 * @note
 *空闲状态下所有灯珠不亮，可激活状态下呈现十字显示，已激活状态下根据击打环数的不同，有不同的显示效果
 * @note
 *小能量机关被激活只有1环亮起；大能量机关击中10环时同时显示1/3/5/7/9/10环的灯珠，其他环数显示对应环数
 * @note   可激活状态2/6/9环亮，还有十字型位置上的灯珠亮
 **/
void fan_screen_show(uint8_t colour, uint8_t fan_state, uint8_t buf_state,
                     uint8_t ring_num) {
  uint8_t r, g, b;
  uint8_t ring;
  uint8_t ring_idx;
  uint16_t index;
  if (fan_state == FAN_IDLE) {
    colour = DARK;
  }
  r = colour_rgb[colour][0];
  g = colour_rgb[colour][1];
  b = colour_rgb[colour][2];

  // 显示相应环数
  if (fan_state == FAN_ACTIVATED) {
    if (buf_state == SMALL_BUF) // 小能量机关击中时，只有1环灯珠亮起
    {
      index = 0;
      for (; index <= ring_led_index[9]; index++) {
        ws2812_set_RGB(0x00, 0x00, 0x00, index, SCREEN_SECTION);
        // ws2812_set_RGB(r, g, b, index, SCREEN_SECTION);
      }
      for (; index <= ring_led_index[10]; index++) {
        // ws2812_set_RGB(0x00, 0x00, 0x00, index, SCREEN_SECTION);
        ws2812_set_RGB(r, g, b, index, SCREEN_SECTION);
      }
    } else if (buf_state == BIG_BUF || buf_state == TEST_BUF) {
      if (ring_num == 10) // 大能量机关击中10环时，1/3/5/7/9/10环都亮
      {
        index = 0;
        for (ring = 2; ring <= 10; ring = ring + 2) {
          for (; index <= ring_led_index[ring]; index++) {
            ws2812_set_RGB(r, g, b, index, SCREEN_SECTION);
          }
          if (ring <= 10) {
            for (; index <= ring_led_index[ring + 1]; index++) {
              ws2812_set_RGB(0x00, 0x00, 0x00, index, SCREEN_SECTION);
            }
          }
        }
        // for ( ; index <= SCREEN_LED_NUM; index++)
        // {
        //     ws2812_set_RGB(r, g, b, index, SCREEN_SECTION);
        // }
      } else // 其他环数，仅对应环灯珠亮

      // 11-ring_num-1是为了将环数对应到index 例如9环的index其实是2

      {
        for (index = 0; index <= ring_led_index[11 - ring_num - 1]; index++) {
          ws2812_set_RGB(0x00, 0x00, 0x00, index, SCREEN_SECTION);
        }
        for (; index <= ring_led_index[11 - ring_num]; index++) {
          ws2812_set_RGB(r, g, b, index, SCREEN_SECTION);
        }
        for (; index < SCREEN_LED_NUM; index++) {
          ws2812_set_RGB(0x00, 0x00, 0x00, index, SCREEN_SECTION);
        }
      }
    }
  } else if (fan_state == FAN_IDLE) {
    for (index = 0; index < SCREEN_LED_NUM; index++) {
      ws2812_set_RGB(r, g, b, index, SCREEN_SECTION);
    }
  } else if (fan_state == FAN_ACTIVATABLE) {
    ring_idx = 0;
    index = 0;
    uint8_t cross_index_index = 0;
    for (ring_idx = 1; ring_idx <= 10; ring_idx++) {
      if (ring_idx == 2 || ring_idx == 5 || ring_idx == 9) {
        for (; index <= ring_led_index[ring_idx]; index++) {
          ws2812_set_RGB(r, g, b, index, SCREEN_SECTION);
        }
      }
      // else if (ring == 10 || ring == 8)
      // {
      //     for (; index <= ring_led_index[11-ring]; index++)
      //     {
      //         ws2812_set_RGB(0x00, 0x00, 0x00, index, SCREEN_SECTION);
      //     }
      // }
      else {
        for (; index <= ring_led_index[ring_idx]; index++) {
          if (index == cross_led_index[cross_index_index]) {
            ws2812_set_RGB(r, g, b, index, SCREEN_SECTION);
            cross_index_index++;
          } else {
            ws2812_set_RGB(0x00, 0x00, 0x00, index, SCREEN_SECTION);
          }
        }
      }
    }
  }
  ws2812_fan_refresh(SCREEN_SECTION);
}

/**
 * @brief  刷新整个扇叶灯珠的显示状态
 * @param  colour 显示的颜色——红、蓝、灭
 * @param  fan_state 扇叶状态
 * @param  buf_state 能量机关类型——大能量机关、小能量机关
 * @param  ring_num  击中环数
 * @retval none
 * @note   none
 **/
void fan_show(uint8_t colour, uint8_t fan_state, uint8_t buf_state,
              uint8_t ring_num) {
  fan_frame_show(colour, fan_state);
  fan_screen_show(colour, fan_state, buf_state, ring_num);
    fan_arrow_show(colour, fan_state);
  fan_surround_show(colour, fan_state);
}

/**
 * @brief WS2812颜色数据刷新, 修改颜色值后调用
 */
static void ws2812_refresh(void) {
  // HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)RGB_buffer,
  // WS2812_DATA_LEN);
}

// TODO 具体的通道应由实际接线决定
static void ws2812_fan_refresh(uint8_t led_section) {
  switch (led_section) {
  case FRAME_DOWN_SECTION:
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)frame_down_buffer,
                          WS2812_FRAME_DOWN_DATA_LEN);
    break;
  case FRAME_UP_SECTION:
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_2, (uint32_t *)frame_up_buffer,
                          WS2812_FRAME_UP_DATA_LEN);
    break;
  case SCREEN_SECTION:
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_3, (uint32_t *)screen_buffer,
                          WS2812_SCREEN_DATA_LEN);
    break;
  case ARROW_SECTION:
    HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t *)arrow_buffer,
                          WS2812_ARROW_DATA_LEN);
    break;
  case SURROUND_SECTION:
    HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_4, (uint32_t *)surround_buffer,
                          WS2812_SURROUND_DATA_LEN);
    break;
  default:
    break;
  }
}
