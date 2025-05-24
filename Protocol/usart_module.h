/**
 ******************************************************************************
 * @file    usart_module.h
 * @brief   USART communication module header file
 * @author  YourName
 * @date    2025-05-08
 * @version V1.0
 * @note    Provides USART communication APIs
 ******************************************************************************
 */

#ifndef __USART_MODULE_H__
#define __USART_MODULE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "pcu_ccu_protocol.h"


#pragma pack(push, 1)  // 1字节对齐，避免结构体填充




/*---------------------功能码--------------------------- */
#define 	LED_CMD		0x0001




#pragma pack(pop)  // 恢复默认对齐方式
// USART Module APIs
uint16_t Build_UART_Frame(uint16_t func_code, uint8_t *data, uint16_t data_len, uint8_t *tx_buf);
bool Parse_UART_Frame(uint8_t *rx_buf, uint16_t rx_len);
#ifdef __cplusplus
}
#endif

#endif // __USART_MODULE_H__
