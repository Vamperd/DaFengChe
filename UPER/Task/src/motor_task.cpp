/**
 *******************************************************************************
 * @file              :motor_task.cpp
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
/* Includes ------------------------------------------------------------------*/
#include "motor_task.hpp"
#include "arm_math.h"
#include "rng.h"
/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
 float spd_ref_=0.0f;
 float spd_fdb_=0.0f;
 float ang_ref_=0.0f;
 float ang_fdb_=0.0f;
/* External variables --------------------------------------------------------*/
extern hw_motor::Motor *Roll_motor_ptr;
extern hw_pid::MultiNodesPid *small_buf_pid_ptr;
extern hw_pid::MultiNodesPid *Big_buf_pid_ptr;

extern float cur_ref_;
extern float cur_fdb_;
extern uint8_t generate_flag;
/* Private function prototypes -----------------------------------------------*/

void Motor_Task(uint8_t buf_state, uint32_t tick_task, int8_t dirction){
    Motor_Update();
        switch (buf_state)
    {
        case BIG_BUF  : Moter_Big_Buf_Task(tick_task,dirction); break;
        case SMALL_BUF: Moter_Small_Buf_Task(dirction)     ; break;
        case SAFE_BUF : Motor_Stop()               ; break;         // 电机不动，就是CAN不发送信息大电机
        default: break;
    }
}

void Motor_Update(){
    spd_fdb_ = Roll_motor_ptr->vel();
    ang_fdb_ = Roll_motor_ptr->angle();
}

void Moter_Big_Buf_Task(uint32_t tick_task, int8_t dirction){
    double work_time = (double)tick_task / 500.0;
    static float a;
    static float w;
    static float b;
    if(generate_flag == 1)
    {
        a = Get_Rand_Num(BIG_BUF_A_MIN, BIG_BUF_A_MAX);
        w = Get_Rand_Num(BIG_BUF_W_MIN, BIG_BUF_W_MAX);
        b = BIG_BUF_B_ERR - a;
        generate_flag = 0;
    }
    float ang_ref_=ang_fdb_+dirction*(a*arm_sin_f32(w*work_time)+b)*0.002f;
    float ref = ang_ref_;
    float fdb[2] = {ang_fdb_, spd_fdb_};
    Big_buf_pid_ptr->calc(&ref, fdb, nullptr, &cur_ref_);
    spd_ref_ = Big_buf_pid_ptr->getDatasAt(1).ref;
}

void Moter_Small_Buf_Task(int8_t dirction){
    spd_ref_=dirction * SMALL_BUF_SPEED;
    float ref = spd_ref_;
    float fdb[1] = {spd_fdb_};
    small_buf_pid_ptr->calc(&ref, fdb, nullptr, &cur_ref_);
}

void Motor_Stop(void){
    spd_ref_=0.0f;
    float ref = spd_ref_;
    float fdb[1] = {spd_fdb_};
    small_buf_pid_ptr->calc(&ref, fdb, nullptr, &cur_ref_);
    if(spd_fdb_<=1.0f){
        cur_ref_=0.0f;
    }
}

float Get_Rand_Num(float min, float max){
    uint32_t round;
    float rand_num;
    round = (max - min) * 1000.0;
    rand_num = min + 1.0 * (HAL_RNG_GetRandomNumber(&hrng) % round) / 1000.0;
    return rand_num;
}
//arm_sin_f32


