/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-20
 * @brief       ����� ʵ��
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
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "key.h"
#include "systick.h"
#include <stdio.h>
#include "usart_module.h"

uint8_t tx_buf[256];
uint8_t payload[1] = {0x03};  // ʾ������
extern UART_HandleTypeDef huart1;
int main(void)
{
	uint8_t len = 0;
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);
	USART1_Init();
	led_init();
	len = Build_UART_Frame(LED1_ON_CMD,payload,1, tx_buf);
	
    uint16_t i = 0;
	printf("tx_buf:");
	for(i = 0; i < len; i++)
	{
		printf("%02x ",tx_buf[i]);
	}
	printf("\n");
	led_off(0);  // ��������һ������ led_on(index)
	led_off(1); 
    while (1)
    {

        //printf("i = %d\r\n", i);
        HAL_Delay(500); // �������һ��
    }
}


