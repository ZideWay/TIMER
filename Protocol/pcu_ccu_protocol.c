/*******************************************************************************
 *          Copyright (c) 2024-2032,  Co., Ltd.
 *                              All Right Reserved.
 * @file pcu_ccu_protocol.c
 * @note 
 * @brief 
 * 
 * @author   
 * @date     
 * @version ͨ��Э��Ӧ�ñ�̽ӿ�
 * 
 * @Description 
 *              
 * 
 * @note History:        
 * @note     <author>   <time>    <version >   <desc>
 * @note  
 * @warning
 *******************************************************************************/
    

#include "pcu_ccu_protocol.h"
#include <stdio.h>
#include <string.h>
#include "led.h"

/* ���ڽ��ձ��� */
#define COMMAND_TABLE_LEN (1u)

const CMD_TABLE command_table[] =
{
    /*  */
    {LED1_ON_CMD,             parse_led_frame},         //����led
	
};

/**
 * @brief 
 * @param[in]
 * @param[out]   
 * @return
 * @note 
 */
uint16_t bytes_to_uint16_le(const uint8_t bytes[2]) 
{
    uint16_t value = 0;
    memcpy(&value, bytes, 2);   // ���ֽ����鸴�Ƶ�uint16_t������
    return value;               // ��С�˸�ʽ����
}

/**
 * @brief 
 * @param[in]
 * @param[out]   
 * @return
 * @note 
 */
uint32_t bytes_to_uint32_le(const uint8_t bytes[4]) 
{
    uint32_t value = 0;
    memcpy(&value, bytes, 4);   // ���ֽ����鸴�Ƶ�uint32_t������
    return value;               // ��С�˸�ʽ����
}

uint8_t g_send_frame[1024] = {0};
/**
 * @brief 
 * @param[in]
 * @param[out]   
 * @return
 * @note 
 */
//void pcu_send_frame(uint8_t *frame, int frame_len) 
//{
//    rt_kprintf("send frame: ");
//    for (int i = 0; i < frame_len; i++) 
//    {
//        rt_kprintf("0x%02X ", frame[i]);
//    }
//    rt_kprintf("\n");
//    
//    if( (rt_strstr(gs_version_info.hardware_version,"HWB0173") != NULL) )
//    {
//        ch395q_cmd_write_send_buf_sn(CH395Q_SOCKET_0, frame, frame_len);
//    }
//    else
//    {
//        rs485_send(inst_slave,frame,frame_len);
//    } 
//}


/**
 * @brief  
 * @param[in]
 * @param[out]   
 * @return
 * @note 
 */
int parse_led_frame(UART_Frame_t *recv_data_domain_frame, uint16_t recv_data_domain_len)
{
    UART_Frame_t send_data_domain_frame;
    
    uint8_t     led_status   = 0;
    uint16_t    payload_len    = 0;
    
    led_status = recv_data_domain_frame->data[0];
	
	for (int i = 0; i < 8; i++)
    {
        if (led_status & (1 << i))
        {
            // λΪ1��������i��LED
            led_on(i);  // ��������һ������ led_on(index)
        }
        else
        {
            // λΪ0���رյ�i��LED
            
			led_off(i); // ��������һ������ led_off(index)
        }
    }
    return 0;
}


int dispatch_command(UART_Frame_t *recv_frame, uint16_t frame_len)
{
    for (uint8_t i = 0; i < COMMAND_TABLE_LEN; i++)
    {
        if (command_table[i].cmd_code == recv_frame->func_code)
        {
            command_table[i].cmd_function(recv_frame, frame_len);
            break;
        }
    }
    return 0;
}


