/**
 ****************************************************************************************************
 * @file        led.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-17
 * @brief       LED 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200417
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef _KEY_H
#define _KEY_H
#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 引脚 定义 */

#define KEY_GPIO_PORT                  GPIOE
#define KEY_GPIO_PIN                   GPIO_PIN_4
#define KEY_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)             /* PE口时钟使能 */

#define DEBOUNCE_TIME   20    // 20ms
#define LONG_PRESS_TIME 1000  // 1000ms


/******************************************************************************************/
/* KEY端口定义 */
typedef enum {
    KEY_RELEASE = 0,
    KEY_DEBOUNCE,
    KEY_SHORT_PRESS,
    KEY_LONG_PRESS
} KeyState;


/******************************************************************************************/
/* 外部接口函数*/
void Key_Init(void);                                                                        /* 初始化 */
void Key_Scan(void);  // 每1ms调用一次
uint8_t Key_WasShortPressed(void);
uint8_t Key_WasLongPressed(void);

#endif
