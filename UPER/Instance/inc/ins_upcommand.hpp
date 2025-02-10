/**
 *******************************************************************************
 * @file              :ins_upcommand.hpp
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
#ifndef INS_UPCOMMAND_HPP_
#define INS_UPCOMMAND_HPP_
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported macro ------------------------------------------------------------*/
#define FAN_IDLE        0           //扇叶显示状态——空闲
#define FAN_ACTIVATABLE 1           //扇叶显示状态——可激活
#define FAN_ACTIVATED   2           //扇叶显示状态——已激活
#define FAN_ILLEGAL     3           //扇叶被非法击中

#define BIG_BUF     0
#define SMALL_BUF   1
#define SAFE_BUF    2

#define BLUE     0
#define RED      1
#define DARK     2

class fan_state_t {
public:
    uint8_t ring_num;
    uint8_t fan_show_state;

    // 构造函数
    fan_state_t(uint8_t ring = 0, uint8_t state = 0) 
        : ring_num(ring), fan_show_state(state) {}

    // 可以添加一些成员函数来操作这些数据
    void set_state(uint8_t ring, uint8_t state) {
        ring_num = ring;
        fan_show_state = state;
    }
};

class buf_t {
public:
    uint8_t colour;
    uint8_t buf_state;
    fan_state_t fan_feedback[5];   // 从下位机得到的反馈信号
    fan_state_t fan_target[5];     // 扇叶的目标运行状态

    uint8_t last_colour;
    uint8_t last_buf_state;
    fan_state_t last_fan_feedback[5];
    fan_state_t last_fan_target[5];

    // 构造函数
    buf_t(uint8_t color, uint8_t b_state)
        : colour(color), buf_state(b_state), last_colour(0), last_buf_state(0) {
        // 默认构造数组中的元素
        for (int i = 0; i < 5; ++i) {
            fan_feedback[i] = fan_state_t();
            fan_target[i] = fan_state_t();
            last_fan_feedback[i] = fan_state_t();
            last_fan_target[i] = fan_state_t();
        }
    }

    // 可以添加一些成员函数来操作这些数据
    void reset() {
        colour = 2;
        buf_state = 2;
        last_colour = 2;
        last_buf_state = 2;

        for (int i = 0; i < 5; ++i) {
            fan_feedback[i] = fan_state_t();
            fan_target[i] = fan_state_t();
            last_fan_feedback[i] = fan_state_t();
            last_fan_target[i] = fan_state_t();
        }
    }

    // 例如，获取某个扇叶的反馈状态
    fan_state_t get_fan_feedback(int index) {
        if (index >= 0 && index < 5) {
            return fan_feedback[index];
        }else{
        return fan_state_t();  // 返回默认值
    }
    }
};


/* Exported types ------------------------------------------------------------*/
#endif /* INS_UPCOMMAND_HPP_ */ 
