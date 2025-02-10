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
#define TEST_BUF    2

#define BLUE     0
#define RED      1
#define DARK     2



class UpCommand {
private:
    uint8_t fan_show_state;
    uint8_t buf_state;
    uint8_t colour;
    uint8_t raw_command;
public:
    UpCommand(uint8_t fan_show_state_, uint8_t buf_state_, uint8_t colour_, uint8_t raw_command_);
    void SetFanShowState(uint8_t fan_show_state_);
    void SetBufState(uint8_t buf_state_);
    void SetColour(uint8_t colour_);
    void SetRawCommand(uint8_t raw_command_);
    uint8_t GetFanShowState(){return fan_show_state;};
    uint8_t GetBufState(){return buf_state;};
    uint8_t GetColour(){return colour;};
    uint8_t GetRawCommand(){return raw_command;};
};

/* Exported types ------------------------------------------------------------*/
#endif /* INS_UPCOMMAND_HPP_ */ 
