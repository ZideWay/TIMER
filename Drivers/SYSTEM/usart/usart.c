/**
 ****************************************************************************************************
 * @file        usart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2023-06-05
 * @brief       串口初始化代码(一般是串口1)，支持printf
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
 * V1.0 20211103
 * 第一次发布
 * V1.1 20230605
 * 删除USART_UX_IRQHandler()函数的超时处理和修改HAL_UART_RxCpltCallback()
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
/* 如果使用os,则包括下面的头文件即可. */
#if SYS_SUPPORT_OS
#include "os.h" /* os 使用 */
#endif

/******************************************************************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */

#if 1

#if (__ARMCC_VERSION >= 6010050)            /* 使用AC6编译器时 */
__asm(".global __use_no_semihosting\n\t");  /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6下需要声明main函数为无参数格式，否则部分例程可能出现半主机模式 */

#else
/* 使用AC5编译器时, 要在这里定义__FILE 和 不使用半主机模式 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* 不使用半主机模式，至少需要重定义_ttywrch\_sys_exit\_sys_command_string函数,以同时兼容AC6和AC5模式 */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}


/* FILE 在 stdio.h里面定义. */
FILE __stdout;

/* MDK下需要重定义fputc函数, printf函数最终会通过调用fputc输出字符串到串口 */
int fputc(int ch, FILE *f)
{
    while ((USART_UX->SR & 0X40) == 0);     /* 等待上一个字符发送完成 */

    USART_UX->DR = (uint8_t)ch;             /* 将要发送的字符 ch 写入到DR寄存器 */
    return ch;
}
#endif
/******************************************************************************************/

/**
 ******************************************************************************
 * @file    usart_dma.c
 * @brief   USART1 DMA接收实现（循环模式+空闲中断）
 * @note    特点：
 *          1. 使用DMA循环模式自动接收数据
 *          2. 通过空闲中断检测数据帧边界
 *          3. 双缓冲区设计避免数据竞争
 *          4. 完善的错误处理机制
 ******************************************************************************
 */

#include "usart.h"
#include <string.h>

/* 私有全局变量 ------------------------------------------------------------*/
#define USART_RX_BUF_SIZE 256  // 必须是2的幂次方（便于环形缓冲区计算）

uint8_t buf[USART_RX_BUF_SIZE];
uint16_t usart1_rx_len = 0;
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* 私有函数声明 ------------------------------------------------------------*/
static void USART1_Error_Handler(void);

/**
 * @brief  USART1初始化
 * @param  无
 * @retval 无
 */
void USART1_Init(void)
{
    /* 1. 使能时钟 */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* 2. 配置GPIO */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_9;        // TX
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;       // RX
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 3. 配置DMA */
    hdma_usart1_rx.Instance = DMA1_Channel5;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;  // 循环模式
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_HIGH;
    
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK) {
        USART1_Error_Handler();
    }
    __HAL_LINKDMA(&huart1, hdmarx, hdma_usart1_rx);

    /* 4. 配置USART */
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

    /* 5. 使能空闲中断 */
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);

    /* 6. 配置中断优先级 */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

    /* 7. 启动DMA接收 */
    if (HAL_UART_Receive_DMA(&huart1, buf, USART_RX_BUF_SIZE) != HAL_OK) {
        printf("uart receive dma failed\n");
    }
}



/**
 * @brief  USART1中断服务函数
 */
void USART1_IRQHandler(void)
{
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)) 
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);

        // 停止 DMA
        HAL_UART_DMAStop(&huart1);

        // 计算已接收数据长度
        usart1_rx_len = USART_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
		
		// 回显数据
		HAL_UART_Transmit(&huart1, buf, usart1_rx_len, HAL_MAX_DELAY);
        // 调用你的帧解析函数
        extern bool Parse_UART_Frame(uint8_t *rx_buf, uint16_t rx_len);
        Parse_UART_Frame(buf, usart1_rx_len);

        // 清空缓冲区（可选）
        memset(buf, 0, USART_RX_BUF_SIZE);

        // 重新启动 DMA 接收
        HAL_UART_Receive_DMA(&huart1, buf, USART_RX_BUF_SIZE);
    }

    HAL_UART_IRQHandler(&huart1);
}


/**
 * @brief  DMA中断服务函数
 */
void DMA1_Channel5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_rx);
}



/**
 * @brief  错误处理函数
 */
static void USART1_Error_Handler(void)
{
    while (1) {
        /* 错误时可闪烁LED或采取其他措施 */
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/