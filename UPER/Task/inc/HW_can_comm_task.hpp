#ifndef _HW_CAN_H_
#define _HW_CAN_H_
#ifdef __cplusplus
extern "C"
{
#endif
/* ------------------------------ Include ------------------------------ */
// #include "stm32f103xe.h"
// #include "stm32f1xx.h"
#include "main.h"
// #include "system_user.hpp"

#include "can.h"
    /* ------------------------------ Macro Definition ------------------------------ */
// #define SLV_BASEADDR    0x100

#define MASTER_BASEADDR     0x1f1
#define SLV_BASEADDR        0x101// 用于将1-5映射为0-4，便于数组管理
#define CAN_TSET_ID         0x2f1

// #define FAN_IDLE        0           //扇叶显示状态——不可激活
// #define FAN_ACTIVATABLE 1           //扇叶显示状态——可激活
// #define FAN_ACTIVATED   2           //扇叶显示状态——已激活
// #define FAN_ILLEGAL     3           //扇叶被非法击中
    /* ------------------------------ Type Definition ------------------------------ */
    /* ------------------------------ Extern Global Variable ------------------------------ */

    /* ------------------------------ Function Declaration (used in other .c files) ------------------------------ */

void CanFilter_Init(CAN_HandleTypeDef* hcan_init);
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan_rx);
void CAN_Send_Msg(CAN_HandleTypeDef *hcan, uint8_t *msg, uint32_t id, uint8_t len);

#ifdef __cplusplus
}
#endif
#endif
