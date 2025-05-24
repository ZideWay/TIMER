/**
 ****************************************************************************************************
 * @file        usart.c
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

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
/* ���ʹ��os,����������ͷ�ļ�����. */
#if SYS_SUPPORT_OS
#include "os.h" /* os ʹ�� */
#endif

/******************************************************************************************/
/* �������´���, ֧��printf����, ������Ҫѡ��use MicroLIB */

#if 1

#if (__ARMCC_VERSION >= 6010050)            /* ʹ��AC6������ʱ */
__asm(".global __use_no_semihosting\n\t");  /* ������ʹ�ð�����ģʽ */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6����Ҫ����main����Ϊ�޲�����ʽ�����򲿷����̿��ܳ��ְ�����ģʽ */

#else
/* ʹ��AC5������ʱ, Ҫ�����ﶨ��__FILE �� ��ʹ�ð�����ģʽ */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* ��ʹ�ð�����ģʽ��������Ҫ�ض���_ttywrch\_sys_exit\_sys_command_string����,��ͬʱ����AC6��AC5ģʽ */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* ����_sys_exit()�Ա���ʹ�ð�����ģʽ */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}


/* FILE �� stdio.h���涨��. */
FILE __stdout;

/* MDK����Ҫ�ض���fputc����, printf�������ջ�ͨ������fputc����ַ��������� */
int fputc(int ch, FILE *f)
{
    while ((USART_UX->SR & 0X40) == 0);     /* �ȴ���һ���ַ�������� */

    USART_UX->DR = (uint8_t)ch;             /* ��Ҫ���͵��ַ� ch д�뵽DR�Ĵ��� */
    return ch;
}
#endif
/******************************************************************************************/

/**
 ******************************************************************************
 * @file    usart_dma.c
 * @brief   USART1 DMA����ʵ�֣�ѭ��ģʽ+�����жϣ�
 * @note    �ص㣺
 *          1. ʹ��DMAѭ��ģʽ�Զ���������
 *          2. ͨ�������жϼ������֡�߽�
 *          3. ˫��������Ʊ������ݾ���
 *          4. ���ƵĴ��������
 ******************************************************************************
 */

#include "usart.h"
#include <string.h>

/* ˽��ȫ�ֱ��� ------------------------------------------------------------*/
#define USART_RX_BUF_SIZE 256  // ������2���ݴη������ڻ��λ��������㣩

uint8_t buf[USART_RX_BUF_SIZE];
uint16_t usart1_rx_len = 0;
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* ˽�к������� ------------------------------------------------------------*/
static void USART1_Error_Handler(void);

/**
 * @brief  USART1��ʼ��
 * @param  ��
 * @retval ��
 */
void USART1_Init(void)
{
    /* 1. ʹ��ʱ�� */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* 2. ����GPIO */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_9;        // TX
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;       // RX
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 3. ����DMA */
    hdma_usart1_rx.Instance = DMA1_Channel5;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;  // ѭ��ģʽ
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_HIGH;
    
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK) {
        USART1_Error_Handler();
    }
    __HAL_LINKDMA(&huart1, hdmarx, hdma_usart1_rx);

    /* 4. ����USART */
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        printf("uart_init failed\n");
    }

    /* 5. ʹ�ܿ����ж� */
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);

    /* 6. �����ж����ȼ� */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

    /* 7. ����DMA���� */
    if (HAL_UART_Receive_DMA(&huart1, buf, USART_RX_BUF_SIZE) != HAL_OK) {
        printf("uart receive dma failed\n");
    }
}



/**
 * @brief  USART1�жϷ�����
 */
void USART1_IRQHandler(void)
{
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)) 
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);

        // ֹͣ DMA
        HAL_UART_DMAStop(&huart1);

        // �����ѽ������ݳ���
        usart1_rx_len = USART_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
		
		// ��������
		HAL_UART_Transmit(&huart1, buf, usart1_rx_len, HAL_MAX_DELAY);
        // �������֡��������
        extern bool Parse_UART_Frame(uint8_t *rx_buf, uint16_t rx_len);
        Parse_UART_Frame(buf, usart1_rx_len);

        // ��ջ���������ѡ��
        memset(buf, 0, USART_RX_BUF_SIZE);

        // �������� DMA ����
        HAL_UART_Receive_DMA(&huart1, buf, USART_RX_BUF_SIZE);
    }

    HAL_UART_IRQHandler(&huart1);
}


/**
 * @brief  DMA�жϷ�����
 */
void DMA1_Channel5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_rx);
}



/**
 * @brief  ��������
 */
static void USART1_Error_Handler(void)
{
    while (1) {
        /* ����ʱ����˸LED���ȡ������ʩ */
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/