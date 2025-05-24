/*******************************************************************************
 *          Copyright (c) 2024-2032,  Co., Ltd.
 *                              All Right Reserved.
 * @file pcu_ccu_protocol.c
 * @note 
 * @brief 
 * 
 * @author   
 * @date     
 * @version
 * 
 * @Description 
 *              
 * 
 * @note History:        
 * @note     <author>   <time>    <version >   <desc>
 * @note  
 * @warning
 *******************************************************************************/
#ifndef __PCU_CCU_PROTOCOL_H__
#define __PCU_CCU_PROTOCOL_H__
#include <stdint.h>
#include "led.h"
#include "usart_module.h"
#ifdef __cplusplus
extern "C"
{
#endif


#define MARSTER_ADRESS                  0x55
/*  */
#define LED1_ON_CMD               	0x0000      //����led
/*  */
#define STOP_IMD_CMD			    0x0002      //ֹͣ��Ե���
/*  */
#define CTRL_STRATEGY_CMD			0x0003      //�̵�������
/*  */
#define SOFT_RESET_CMD				0x0004      //�����λ
/*  */
#define USER_MODE_CMD               0x0005      //����������û�ģʽ(δ��,Ŀǰ�Ĳ������ϵ��������ģʽ��ת���û�ģʽ)
/*  */
#define ASSEMBLING_MODE_CMD			0x0006      //�������������ģʽ(δ��,Ŀǰ�Ĳ������ϵ�ͽ�������ģʽ)
/*  */
#define START_CHARGING_CMD			0x0007      //�������������(δ��)
/*  */
#define STOP_CHARGING_CMD			0x0008      //������ֹͣ���(δ��)
/*  */
#define FACTORY_RESET_CMD			0x000E      //�豸�ָ���������(δ��)
/*  */
#define TIMING_CMD			        0x0020      //�豸Уʱ
/*  */
#define GET_VERSION_CMD			    0x0030      //��ѯ�豸�汾��Ϣ
/*  */
#define GET_KEY_CMD                 0x0031      //��ѯ������Կ(δ��)
/*  */
#define GET_OCMF_CMD		        0x0032      //��ѯ������OCMF����(δ��)
/*  */
#define GET_DATA_CMD			    0x0033      //ʵʱ���ݲ�ѯ
/*  */
#define SEND_START_OTA_CMD		    0x0040      //�豸����ָ��
/*  */
#define SEND_DATA_OTA_CMD			0x0041      //�豸��������

#define FRAME_HEADER_1  0x5A
#define FRAME_HEADER_2  0xA5
#define FRAME_TAIL_1    0x69
#define FRAME_TAIL_2    0x69

typedef struct {
    uint8_t  header[2];    // ֡ͷ 0x5A 0xA5
    uint16_t func_code;    // ������
	uint16_t length;       // ֡���ȣ�����֡ͷ��֡β��CRC�ȣ�
    uint8_t  data[256];    // �ɱ䳤�����ݣ��������飩
	uint16_t crc;			// crcУ��
	uint8_t  tailer[2];    // ֡ͷ 0x69 0x69
} UART_Frame_t;

typedef struct
{
	/* ������ */
    uint16_t cmd_code;
	/*  */
    int (*cmd_function)(UART_Frame_t *data_domain_frame, uint16_t recv_data_domain_len);	
}CMD_TABLE;



/**
 * @brief 
 * @param[in]
 * @param[out]   
 * @return
 * @note 
 */
extern int pcu_pro_cmd_from_ccu(UART_Frame_t *recv_data_domain_frame, uint16_t recv_payload_len);

extern void pack_pcu_packet(uint8_t address, UART_Frame_t *domain_frame, size_t payload_len, uint8_t *frame, int *send_frame_len);

extern void pcu_send_frame(uint8_t *frame, int frame_len);

int parse_led_frame(UART_Frame_t *recv_data_domain_frame, uint16_t recv_data_domain_len);
#ifdef __cplusplus
}
#endif
#endif

