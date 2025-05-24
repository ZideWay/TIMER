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
#define LED1_ON_CMD               	0x0000      //启动led
/*  */
#define STOP_IMD_CMD			    0x0002      //停止绝缘检测
/*  */
#define CTRL_STRATEGY_CMD			0x0003      //继电器控制
/*  */
#define SOFT_RESET_CMD				0x0004      //软件复位
/*  */
#define USER_MODE_CMD               0x0005      //东鸿电表进入用户模式(未做,目前的策略是上电就由配置模式跳转到用户模式)
/*  */
#define ASSEMBLING_MODE_CMD			0x0006      //东鸿电表进入配置模式(未做,目前的策略是上电就进入配置模式)
/*  */
#define START_CHARGING_CMD			0x0007      //东鸿电表启动充电(未做)
/*  */
#define STOP_CHARGING_CMD			0x0008      //东鸿电表停止充电(未做)
/*  */
#define FACTORY_RESET_CMD			0x000E      //设备恢复出厂设置(未做)
/*  */
#define TIMING_CMD			        0x0020      //设备校时
/*  */
#define GET_VERSION_CMD			    0x0030      //查询设备版本信息
/*  */
#define GET_KEY_CMD                 0x0031      //查询东鸿电表公钥(未做)
/*  */
#define GET_OCMF_CMD		        0x0032      //查询东鸿电表OCMF数据(未做)
/*  */
#define GET_DATA_CMD			    0x0033      //实时数据查询
/*  */
#define SEND_START_OTA_CMD		    0x0040      //设备升级指令
/*  */
#define SEND_DATA_OTA_CMD			0x0041      //设备升级数据

#define FRAME_HEADER_1  0x5A
#define FRAME_HEADER_2  0xA5
#define FRAME_TAIL_1    0x69
#define FRAME_TAIL_2    0x69

typedef struct {
    uint8_t  header[2];    // 帧头 0x5A 0xA5
    uint16_t func_code;    // 功能码
	uint16_t length;       // 帧长度（包括帧头、帧尾、CRC等）
    uint8_t  data[256];    // 可变长度数据（柔性数组）
	uint16_t crc;			// crc校验
	uint8_t  tailer[2];    // 帧头 0x69 0x69
} UART_Frame_t;

typedef struct
{
	/* 功能码 */
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

