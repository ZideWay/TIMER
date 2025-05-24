/**
 ****************************************************************************************************
 * @file        led.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-17
 * @brief       LED ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200417
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#ifndef _KEY_H
#define _KEY_H
#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* ���� ���� */

#define KEY_GPIO_PORT                  GPIOE
#define KEY_GPIO_PIN                   GPIO_PIN_4
#define KEY_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)             /* PE��ʱ��ʹ�� */

#define DEBOUNCE_TIME   20    // 20ms
#define LONG_PRESS_TIME 1000  // 1000ms


/******************************************************************************************/
/* KEY�˿ڶ��� */
typedef enum {
    KEY_RELEASE = 0,
    KEY_DEBOUNCE,
    KEY_SHORT_PRESS,
    KEY_LONG_PRESS
} KeyState;


/******************************************************************************************/
/* �ⲿ�ӿں���*/
void Key_Init(void);                                                                        /* ��ʼ�� */
void Key_Scan(void);  // ÿ1ms����һ��
uint8_t Key_WasShortPressed(void);
uint8_t Key_WasLongPressed(void);

#endif
