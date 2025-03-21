/**
 *******************************************************************************
 * @file              :ins_be_shot.hpp
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
#ifndef INS_BE_SHOT_HPP_
#define INS_BE_SHOT_HPP_
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported macro ------------------------------------------------------------*/
#define HIT     0 //#TODO 由硬件决定
#define MISS    1

//#TODO 根据实际的接线情况，下面IO口与环数的对应顺序需要更改
#define ONE_RING_PIN    GPIO_PIN_12
#define TWO_RING_PIN    GPIO_PIN_13
#define THEEE_RING_PIN  GPIO_PIN_14
#define FOUR_RING_PIN   GPIO_PIN_15
#define FIVE_RING_PIN   GPIO_PIN_6
#define SIX_RING_PIN    GPIO_PIN_7
#define SEVEN_RING_PIN  GPIO_PIN_8
#define EIGHT_RING_PIN  GPIO_PIN_9
#define NINE_RING_PIN   GPIO_PIN_8
#define TEN_RING_PIN    GPIO_PIN_9

#define ONE_RING_GPIO_PORT    GPIOB
#define TWO_RING_GPIO_PORT    GPIOB
#define THEEE_RING_GPIO_PORT  GPIOB
#define FOUR_RING_GPIO_PORT   GPIOB
#define FIVE_RING_GPIO_PORT   GPIOC
#define SIX_RING_GPIO_PORT    GPIOC
#define SEVEN_RING_GPIO_PORT  GPIOC
#define EIGHT_RING_GPIO_PORT  GPIOC
#define NINE_RING_GPIO_PORT   GPIOA
#define TEN_RING_GPIO_PORT    GPIOA

/* Exported types ------------------------------------------------------------*/
extern uint8_t num_shoot_ring;       //击中环数，-1表示没有击中

void get_shoot_ring(void);
#endif /* INS_BE_SHOT_HPP_ */
