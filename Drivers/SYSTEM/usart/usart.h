/**
 ****************************************************************************************************
 * @file        usart.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2023-06-05
 * @brief       ���ڳ�ʼ������(һ���Ǵ���1)��֧��printf
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
 * V1.0 20211103
 * ��һ�η���
 * V1.1 20230605
 * ɾ��USART_UX_IRQHandler()�����ĳ�ʱ������޸�HAL_UART_RxCpltCallback()
 *
 ****************************************************************************************************
 */

#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "./SYSTEM/sys/sys.h"
#include "stm32f1xx_hal.h"

/******************************************************************************************/
/* ���� �� ���� ���� 
 * Ĭ�������USART1��.
 * ע��: ͨ���޸��⼸���궨��,����֧��USART1~UART5����һ������.
 */
#define USART_TX_GPIO_PORT                  GPIOA
#define USART_TX_GPIO_PIN                   GPIO_PIN_9
#define USART_TX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */

#define USART_RX_GPIO_PORT                  GPIOA
#define USART_RX_GPIO_PIN                   GPIO_PIN_10
#define USART_RX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */

#define USART_UX                            USART1
#define USART_UX_IRQn                       USART1_IRQn
#define USART_UX_IRQHandler                 USART1_IRQHandler
#define USART_UX_CLK_ENABLE()               do{ __HAL_RCC_USART1_CLK_ENABLE(); }while(0)  /* USART1 ʱ��ʹ�� */

/******************************************************************************************/

#define USART_REC_LEN               200         /* �����������ֽ��� 200 */
#define USART_EN_RX                 1           /* ʹ�ܣ�1��/��ֹ��0������1���� */
#define RXBUFFERSIZE   1                        /* �����С */
#define USART1_RX_DMA_LEN 256                        // DMA���ճ���

extern uint8_t g_usart1_dma_rx_buf[USART1_RX_DMA_LEN];


extern UART_HandleTypeDef g_uart1_handle;
void USART1_Init(void);
uint16_t USART1_GetRxLen(void);
uint16_t USART1_Read(uint8_t *pbuf, uint16_t len);
#endif


