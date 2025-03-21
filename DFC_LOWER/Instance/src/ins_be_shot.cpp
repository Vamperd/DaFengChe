/**
 *******************************************************************************
 * @file              :ins_be_shot.cpp
 * @brief             :
 * @history   :
 *  Version     Date            Author          Note
 *  V0.9.0      2025-01-19      <Vamper>        1。<note>
 *******************************************************************************
 * @attention :
 *******************************************************************************
 *  Copyright (c) 2025 Hello World Team,Zhejiang University.
 *  All Rights Reserved.
 *******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "ins_all.hpp"
#include "main_task.hpp"
#include "gpio.h"
#include "ins_upcommand.hpp"
#include "HW_can_comm_task.hpp"
/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t num_shoot_ring = 0;
uint8_t last_num_shoot_ring = 0;
uint32_t last_shoot_tick = 0;
uint32_t shoot_tick = 0;
/* External variables --------------------------------------------------------*/
extern uint32_t tick;
extern UpCommand upcommand;
extern uint8_t valuable_num_shoot_ring;
/* Private function prototypes -----------------------------------------------*/
void get_shoot_ring(void)
{
    // 等待状态机完成后具体判断
    if((upcommand.GetFanShowState())== FAN_ILLEGAL)
    {
        shoot_tick = tick;
        last_shoot_tick = shoot_tick;
        num_shoot_ring = 0;
        ws2812_set_dark();
        return;

    }
    last_num_shoot_ring = num_shoot_ring;

    if(HAL_GPIO_ReadPin(ONE_RING_GPIO_PORT, ONE_RING_PIN) == HIT)
    {
        last_shoot_tick = shoot_tick;
        shoot_tick = tick;
        num_shoot_ring = 1;
    }
    else if(HAL_GPIO_ReadPin(TWO_RING_GPIO_PORT, TWO_RING_PIN) == HIT)
    {
        last_shoot_tick = shoot_tick;
        shoot_tick = tick;
        num_shoot_ring = 2;
    }
    else if(HAL_GPIO_ReadPin(THEEE_RING_GPIO_PORT, THEEE_RING_PIN) == HIT)
    {
        last_shoot_tick = shoot_tick;
        shoot_tick = tick;
        num_shoot_ring = 3;
    }
    else if(HAL_GPIO_ReadPin(FOUR_RING_GPIO_PORT, FOUR_RING_PIN) == HIT)
    {
        last_shoot_tick = shoot_tick;
        shoot_tick = tick;
        num_shoot_ring = 4;
    }
    else if(HAL_GPIO_ReadPin(FIVE_RING_GPIO_PORT, FIVE_RING_PIN) == HIT)
    {
        last_shoot_tick = shoot_tick;
        shoot_tick = tick;
        num_shoot_ring = 5;
    }
    else if(HAL_GPIO_ReadPin(SIX_RING_GPIO_PORT, SIX_RING_PIN) == HIT)
    {
        last_shoot_tick = shoot_tick;
        shoot_tick = tick;
        num_shoot_ring = 6;
    }
    else if(HAL_GPIO_ReadPin(SEVEN_RING_GPIO_PORT, SEVEN_RING_PIN) == HIT)
    {
        last_shoot_tick = shoot_tick;
        shoot_tick = tick;
        num_shoot_ring = 7;
    }
    else if(HAL_GPIO_ReadPin(EIGHT_RING_GPIO_PORT, EIGHT_RING_PIN) == HIT)
    {
        last_shoot_tick = shoot_tick;
        shoot_tick = tick;
        num_shoot_ring = 8;
    }
    else if(HAL_GPIO_ReadPin(NINE_RING_GPIO_PORT, NINE_RING_PIN) == HIT/* || HAL_GPIO_ReadPin(NINE_RING_SPARE_GPIO_Port, NINE_RING_SPARE_Pin) == HIT*/)
    {
        last_shoot_tick = shoot_tick;
        shoot_tick = tick;
        num_shoot_ring = 9;     //由于2号扇叶的12pinGH口被连根拔起，9环引脚的焊盘脱落，所以又引了根线出来
    }
    else if(HAL_GPIO_ReadPin(TEN_RING_GPIO_PORT, TEN_RING_PIN ) == HIT)
    {
        last_shoot_tick = shoot_tick;
        shoot_tick = tick;
        num_shoot_ring = 10;
    }
    else
    {
        num_shoot_ring = 0;            //脱靶
    }
   
//#TODO 将所有的update由class upcomm重新封装，然后在comm_task中完成所有的编码与发送，并且要求所有的修改由upcomm中的函数完成   
   if(num_shoot_ring > 0)    //扇叶状态从可激活到已激活状态
   {
        uint8_t TxData;
        valuable_num_shoot_ring=num_shoot_ring;
        if((upcommand.GetFanShowState()) == FAN_ACTIVATABLE)
        {
            (upcommand.SetFanShowState(FAN_ACTIVATED));
            TxData = ((upcommand.GetFanShowState()) << 4) | num_shoot_ring;
            CAN_Send_Msg(&hcan, &TxData, SLV_BASEADDR + SLV_ID, 1);
        }
        else if(((upcommand.GetFanShowState())== FAN_IDLE||(upcommand.GetFanShowState()==FAN_ACTIVATED))&&((shoot_tick-last_shoot_tick)>500))
        {
            (upcommand.SetFanShowState(FAN_ILLEGAL));
            TxData = ((upcommand.GetFanShowState())<< 4) | num_shoot_ring;
            CAN_Send_Msg(&hcan, &TxData, SLV_BASEADDR + SLV_ID, 1);
        }
        //只有击中时才传输信息到上位机,正常的传输信息为已激活状态+环数，如果是非法状态+环数则说明打中非随机激活扇叶
        
   }
}

