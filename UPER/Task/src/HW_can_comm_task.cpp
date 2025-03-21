/**
 *******************************************************************************
 * @file      : HW_can.c
 * @brief     :
 * @history   :
 *  Version     Date            Author          Note
 *  V0.9.0      yyyy-mm-dd      <author>        1. <note>
 *******************************************************************************
 * @attention :
 *******************************************************************************
 *  Copyright (c) 2023 Hello World Team, Zhejiang University.
 *  All Rights Reserved.
 *******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "HW_can_comm_task.hpp"
#include "stdint.h"
#include "ins_all.hpp"
/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// static CAN_RxHeaderTypeDef rx_header1;
// static uint8_t can1_rx_data[8];
static CAN_RxHeaderTypeDef rx_header2;
static uint8_t can2_rx_data[8];
uint32_t pTxMailbox;
uint8_t tx_can_flag=0;
uint8_t rx_can_flag=0;
/* External variables --------------------------------------------------------*/
extern  buf_t buf;
/* Private function prototypes -----------------------------------------------*/

/**
 * @brief
 * @param        *hcan:
 * @retval       None
 * @note        None
 */
void CanFilter_Init(CAN_HandleTypeDef* hcan_init)
{
    CAN_FilterTypeDef canfilter;

    canfilter.FilterMode = CAN_FILTERMODE_IDLIST;
    canfilter.FilterScale = CAN_FILTERSCALE_16BIT;

    canfilter.FilterActivation = ENABLE;
    canfilter.SlaveStartFilterBank = 14;
    // if (hcan_init == &hcan1)
    // {
    //     canfilter.FilterFIFOAssignment = CAN_FilterFIFO0;

    //     canfilter.FilterMode = CAN_FILTERMODE_IDMASK;
    //     canfilter.FilterScale = CAN_FILTERSCALE_32BIT;
    //     canfilter.FilterIdHigh = 0x0000;
    //     canfilter.FilterIdLow = 0x0000;
    //     canfilter.FilterMaskIdHigh = 0x0000;
    //     canfilter.FilterMaskIdLow = 0x0000;
    //     canfilter.FilterBank = 0;
    //     canfilter.FilterActivation = ENABLE;
    //     if (HAL_CAN_ConfigFilter(hcan_init, &canfilter) != HAL_OK)
    //     {
    //         Error_Handler();
    //     }
    // }
     if (hcan_init == &hcan2)
    {
        canfilter.FilterFIFOAssignment = CAN_FilterFIFO1;
        canfilter.FilterMode = CAN_FILTERMODE_IDMASK;
        canfilter.FilterScale = CAN_FILTERSCALE_32BIT;
        canfilter.FilterIdHigh = 0x0000;
        canfilter.FilterIdLow = 0x0000;
        canfilter.FilterMaskIdHigh = 0x0000;
        canfilter.FilterMaskIdLow = 0x0000;
        canfilter.FilterActivation = ENABLE;
        canfilter.FilterBank = 14;

        if (HAL_CAN_ConfigFilter(hcan_init, &canfilter) != HAL_OK)
        {
            Error_Handler();
        }
    }
}

uint32_t can_rec_times = 0;
uint32_t can_success_times = 0;
uint32_t can_receive_data = 0;

// /**
//  * @brief   CAN中断的回调函数，全部数据解析都在该函数中
//  * @param   hcan为CAN句柄
//  * @retval  none
//  * @note
//  **/
// void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan_rx)
// {
//     if (hcan_rx == &hcan1)
//     {
//         if (HAL_CAN_GetRxMessage(hcan_rx, CAN_RX_FIFO0, &rx_header1, can1_rx_data) == HAL_OK) // 获得接收到的数据头和数据
//         {
//             rx_can_flag++;
//             if (rx_header1.StdId == MASTER_BASEADDR + SLV_ID)
//             { // 帧头校验
//                 // 校验通过进行具体数据处理
//                 upcommand.SetRawCommand(can1_rx_data[0]); // 解析数据
//                 upcommand.SetFanShowState((can1_rx_data[0]>>3)&0x03);
//                 upcommand.SetBufState((can1_rx_data[0]>>1)&0x03);
//                 upcommand.SetColour(can1_rx_data[0]&0x01);
//             }
//         }
//     }
//     HAL_CAN_ActivateNotification(hcan_rx, CAN_IT_RX_FIFO0_MSG_PENDING); // 再次使能FIFO0接收中断
// }

/**
 * @brief   CAN中断的回调函数，全部数据解析都在该函数中
 * @param   hcan为CAN句柄
 * @retval  none
 * @note
 **/
uint8_t can_test[5] = {0};
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan_rx)
{

    if (hcan_rx == &hcan2)
    {
        if (HAL_CAN_GetRxMessage(hcan_rx, CAN_RX_FIFO1, &rx_header2, can2_rx_data) == HAL_OK) // 获得接收到的数据头和数据
        {
            if(rx_header2.StdId >= SLV_BASEADDR && rx_header2.StdId <= SLV_BASEADDR + 0x04)
            {
                uint8_t ID = rx_header2.StdId - SLV_BASEADDR;
                buf.fan_feedback[ID].fan_show_state = can2_rx_data[0] >> 4;
                buf.fan_feedback[ID].ring_num = can2_rx_data[0] & 0x0f;
                //buf.fan_target[ID].fan_show_state = buf.fan_feedback[ID].fan_show_state;
                //buf.fan_feedback[ID].ring_num = RxData_can2[0] & 0x0f;
            }
            else if(rx_header2.StdId >= CAN_TSET_ID && rx_header2.StdId <= CAN_TSET_ID + 0X04)
            {
                can_test[rx_header2.StdId - CAN_TSET_ID] = can2_rx_data[0];       //CAN通信测试
            }
        }
        rx_can_flag++;
    }
    HAL_CAN_ActivateNotification(hcan_rx, CAN_IT_RX_FIFO1_MSG_PENDING); // 再次使能FIFO0接收中断
}

/**
 * @brief   向can总线发送数据，抄官方的
 * @param   hcan为CAN句柄
 * @param	msg为发送数组首地址
 * @param	id为发送报文
 * @param	len为发送数据长度（字节数）
 * @retval  none
 * @note    主控发送都是len=8字节，再加上帧间隔3位，理论上can总线1ms最多传输9帧
 **/
void CAN_Send_Msg(CAN_HandleTypeDef *hcan, uint8_t *msg, uint32_t id, uint8_t len)
{
    CAN_TxHeaderTypeDef TxMessageHeader = {0};
    TxMessageHeader.StdId = id;
    TxMessageHeader.IDE = CAN_ID_STD;
    TxMessageHeader.RTR = CAN_RTR_DATA;
    TxMessageHeader.DLC = len;
    if (HAL_CAN_AddTxMessage(hcan, &TxMessageHeader, msg, &pTxMailbox) != HAL_OK)
    {
        tx_can_flag++;
    }
}
