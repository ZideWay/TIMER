/*******************************************************************************
 *          Copyright (c) 2024-2032,  Co., Ltd.
 *                              All Right Reserved.
 * @file crc_lib.c
 * @note 
 * @brief 
 * 
 * @author   
 * @date     
 * @version
 * 
 * @Description 基于C语言的CRC校验库，包括常用的21个CRC参数模型实现
 *    #### 常用的CRC参数模型
 *
 *   | CRC算法名称        | 多项式公式                                                   | WIDTH | POLY     | INIT     | XOROUT   | REFIN | REFOUT |
 *   | ------------------ | ------------------------------------------------------------ | ----- | -------- | -------- | -------- | ----- | ------ |
 *   | CRC-4/ITU          | x4 +  x + 1                                                  | 4     | 03       | 00       | 00       | TRUE  | TRUE   |
 *   | CRC-5/EPC          | x5 +  x3 + 1                                                 | 5     | 09       | 09       | 00       | FALSE | FALSE  |
 *   | CRC-5/ITU          | x5 +  x4 + x2 + 1                                            | 5     | 15       | 00       | 00       | TRUE  | TRUE   |
 *   | CRC-5/USB          | x5 +  x2 + 1                                                 | 5     | 05       | 1F       | 1F       | TRUE  | TRUE   |
 *   | CRC-6/ITU          | x6 +  x + 1                                                  | 6     | 03       | 00       | 00       | TRUE  | TRUE   |
 *   | CRC-7/MMC          | x7 +  x3 + 1                                                 | 7     | 09       | 00       | 00       | FALSE | FALSE  |
 *   | CRC-8              | x8 +  x2 + x + 1                                             | 8     | 07       | 00       | 00       | FALSE | FALSE  |
 *   | CRC-8/ITU          | x8 +  x2 + x + 1                                             | 8     | 07       | 00       | 55       | FALSE | FALSE  |
 *   | CRC-8/ROHC         | x8 +  x2 + x + 1                                             | 8     | 07       | FF       | 00       | TRUE  | TRUE   |
 *   | CRC-8/MAXIM        | x8 +  x5 + x4 + 1                                            | 8     | 31       | 00       | 00       | TRUE  | TRUE   |
 *   | CRC-16/IBM         | x16 +  x15 + x2 + 1                                          | 16    | 8005     | 0000     | 0000     | TRUE  | TRUE   |
 *   | CRC-16/MAXIM       | x16 +  x15 + x2 + 1                                          | 16    | 8005     | 0000     | FFFF     | TRUE  | TRUE   |
 *   | CRC-16/USB         | x16 +  x15 + x2 + 1                                          | 16    | 8005     | FFFF     | FFFF     | TRUE  | TRUE   |
 *   | CRC-16/MODBUS      | x16 +  x15 + x2 + 1                                          | 16    | 8005     | FFFF     | 0000     | TRUE  | TRUE   |
 *   | CRC-16/CCITT       | x16 +  x12 + x5 + 1                                          | 16    | 1021     | 0000     | 0000     | TRUE  | TRUE   |
 *   | CRC-16/CCITT-FALSE | x16 +  x12 + x5 + 1                                          | 16    | 1021     | FFFF     | 0000     | FALSE | FALSE  |
 *   | CRC-16/X25         | x16 +  x12 + x5 + 1                                          | 16    | 1021     | FFFF     | FFFF     | TRUE  | TRUE   |
 *   | CRC-16/XMODEM      | x16 +  x12 + x5 + 1                                          | 16    | 1021     | 0000     | 0000     | FALSE | FALSE  |
 *   | CRC-16/DNP         | x16 +  x13 + x12 + x11 + x10 + x8 + x6 + x5 +  x2 + 1        | 16    | 3D65     | 0000     | FFFF     | TRUE  | TRUE   |
 *   | CRC-32             | x32 +  x26 + x23 + x22 + x16 + x12 + x11 + x10 +  x8 + x7 + x5 + x4 + x2 + x + 1 | 32    | 04C11DB7 | FFFFFFFF | FFFFFFFF | TRUE  | TRUE   |
 *   | CRC-32/MPEG-2      | x32 +  x26 + x23 + x22 + x16 + x12 + x11 + x10 +  x8 + x7 + x5 + x4 + x2 + x + 1 | 32    | 04C11DB7 | FFFFFFFF | 0        | FALSE | FALSE  |
 *
 *   #### CRC计算工具
 *
 *   在线计算工具：www.ip33.com/crc.html
 *
 *   离线计算工具：
 *
 *   - CRC_Calc v0.1：http://xz.w10a.com/Small/CRCJISUANQI.zip
 *  - 格西CRC计算器：http://www.geshe.com/home/products/GToolbox/bin/GCRC.exe
 *              
 * 
 * @note History:        
 * @note     <author>   <time>    <version >   <desc>
 * @note  
 * @warning
 *******************************************************************************/

#include "crc_lib.h"

/******************************************************************************
 * Name:    CRC-4/ITU           x4+x+1
 * Poly:    0x03
 * Init:    0x00
 * Refin:   True
 * Refout:  True
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
uint8_t crc4_itu(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0;                // Initial value
    while(length--)
    {
        crc ^= *data++;                 // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x0C;// 0x0C = (reverse 0x03)>>(8-4)
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-5/EPC           x5+x3+1
 * Poly:    0x09
 * Init:    0x09
 * Refin:   False
 * Refout:  False
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
uint8_t crc5_epc(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0x48;        // Initial value: 0x48 = 0x09<<(8-5)
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for ( i = 0; i < 8; i++ )
        {
            if ( crc & 0x80 )
                crc = (crc << 1) ^ 0x48;        // 0x48 = 0x09<<(8-5)
            else
                crc <<= 1;
        }
    }
    return crc >> 3;
}

/******************************************************************************
 * Name:    CRC-5/ITU           x5+x4+x2+1
 * Poly:    0x15
 * Init:    0x00
 * Refin:   True
 * Refout:  True
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
uint8_t crc5_itu(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0;                // Initial value
    while(length--)
    {
        crc ^= *data++;                 // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x15;// 0x15 = (reverse 0x15)>>(8-5)
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-5/USB           x5+x2+1
 * Poly:    0x05
 * Init:    0x1F
 * Refin:   True
 * Refout:  True
 * Xorout:  0x1F
 * Note:
 *****************************************************************************/
uint8_t crc5_usb(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0x1F;                // Initial value
    while(length--)
    {
        crc ^= *data++;                 // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x14;// 0x14 = (reverse 0x05)>>(8-5)
            else
                crc = (crc >> 1);
        }
    }
    return crc ^ 0x1F;
}

/******************************************************************************
 * Name:    CRC-6/ITU           x6+x+1
 * Poly:    0x03
 * Init:    0x00
 * Refin:   True
 * Refout:  True
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
uint8_t crc6_itu(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0;         // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x30;// 0x30 = (reverse 0x03)>>(8-6)
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-7/MMC           x7+x3+1
 * Poly:    0x09
 * Init:    0x00
 * Refin:   False
 * Refout:  False
 * Xorout:  0x00
 * Use:     MultiMediaCard,SD,ect.
 *****************************************************************************/
uint8_t crc7_mmc(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0;        // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for ( i = 0; i < 8; i++ )
        {
            if ( crc & 0x80 )
                crc = (crc << 1) ^ 0x12;        // 0x12 = 0x09<<(8-7)
            else
                crc <<= 1;
        }
    }
    return crc >> 1;
}

/******************************************************************************
 * Name:    CRC-8               x8+x2+x+1
 * Poly:    0x07
 * Init:    0x00
 * Refin:   False
 * Refout:  False
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
uint8_t crc8(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0;        // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for ( i = 0; i < 8; i++ )
        {
            if ( crc & 0x80 )
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-8/ITU           x8+x2+x+1
 * Poly:    0x07
 * Init:    0x00
 * Refin:   False
 * Refout:  False
 * Xorout:  0x55
 * Alias:   CRC-8/ATM
 *****************************************************************************/
uint8_t crc8_itu(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0;        // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for ( i = 0; i < 8; i++ )
        {
            if ( crc & 0x80 )
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }
    return crc ^ 0x55;
}

/******************************************************************************
 * Name:    CRC-8/ROHC          x8+x2+x+1
 * Poly:    0x07
 * Init:    0xFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
uint8_t crc8_rohc(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0xFF;         // Initial value
    while(length--)
    {
        crc ^= *data++;            // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xE0;        // 0xE0 = reverse 0x07
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-8/MAXIM         x8+x5+x4+1
 * Poly:    0x31
 * Init:    0x00
 * Refin:   True
 * Refout:  True
 * Xorout:  0x00
 * Alias:   DOW-CRC,CRC-8/IBUTTON
 * Use:     Maxim(Dallas)'s some devices,e.g. DS18B20
 *****************************************************************************/
uint8_t crc8_maxim(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0;         // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for (i = 0; i < 8; i++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x8C;        // 0x8C = reverse 0x31
            else
                crc >>= 1;
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-16/IBM          x16+x15+x2+1
 * Poly:    0x8005
 * Init:    0x0000
 * Refin:   True
 * Refout:  True
 * Xorout:  0x0000
 * Alias:   CRC-16,CRC-16/ARC,CRC-16/LHA
 *****************************************************************************/
uint16_t crc16_ibm(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0;        // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;        // 0xA001 = reverse 0x8005
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-16/MAXIM        x16+x15+x2+1
 * Poly:    0x8005
 * Init:    0x0000
 * Refin:   True
 * Refout:  True
 * Xorout:  0xFFFF
 * Note:
 *****************************************************************************/
uint16_t crc16_maxim(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0;        // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;        // 0xA001 = reverse 0x8005
            else
                crc = (crc >> 1);
        }
    }
    return ~crc;    // crc^0xffff
}

/******************************************************************************
 * Name:    CRC-16/USB          x16+x15+x2+1
 * Poly:    0x8005
 * Init:    0xFFFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0xFFFF
 * Note:
 *****************************************************************************/
uint16_t crc16_usb(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0xffff;        // Initial value
    while(length--)
    {
        crc ^= *data++;            // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;        // 0xA001 = reverse 0x8005
            else
                crc = (crc >> 1);
        }
    }
    return ~crc;    // crc^0xffff
}

/******************************************************************************
 * Name:    CRC-16/MODBUS       x16+x15+x2+1
 * Poly:    0x8005
 * Init:    0xFFFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0x0000
 * Note:
 *****************************************************************************/
uint16_t crc16_modbus(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0xffff;        // Initial value
    while(length--)
    {
        crc ^= *data++;            // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;        // 0xA001 = reverse 0x8005
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-16/CCITT        x16+x12+x5+1
 * Poly:    0x1021
 * Init:    0x0000
 * Refin:   True
 * Refout:  True
 * Xorout:  0x0000
 * Alias:   CRC-CCITT,CRC-16/CCITT-TRUE,CRC-16/KERMIT
 *****************************************************************************/
uint16_t crc16_ccitt(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0;        // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x8408;        // 0x8408 = reverse 0x1021
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-16/CCITT-FALSE   x16+x12+x5+1
 * Poly:    0x1021
 * Init:    0xFFFF
 * Refin:   False
 * Refout:  False
 * Xorout:  0x0000
 * Note:
 *****************************************************************************/
uint16_t crc16_ccitt_false(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0xffff;        //Initial value
    while(length--)
    {
        crc ^= (uint16_t)(*data++) << 8; // crc ^= (uint6_t)(*data)<<8; data++;
        for (i = 0; i < 8; ++i)
        {
            if ( crc & 0x8000 )
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-16/X25          x16+x12+x5+1
 * Poly:    0x1021
 * Init:    0xFFFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0XFFFF
 * Note:
 *****************************************************************************/
uint16_t crc16_x25(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0xffff;        // Initial value
    while(length--)
    {
        crc ^= *data++;            // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x8408;        // 0x8408 = reverse 0x1021
            else
                crc = (crc >> 1);
        }
    }
    return ~crc;                // crc^Xorout
}

/****************************************************************
 * Name:   CRC-16/XMODEM  x16+x12+x5+1
 * Poly:   0x1021
 * Init:   0x0000
 * Refin:  False
 * Refout: False
 * Xorout: 0x0000
 * Alias:  CRC-16/ZMODEM,CRC-16/ACORN
 **************************************************************/
uint16_t crc16_xmodem(uint8_t *data, uint16_t length)
{
    //X^16+X^12+X^5+1 total 16 effective bits without X^16.
    uint16_t crc_poly = 0x1021;
    uint16_t crc = 0x0000;
    while(length--)
    {
        crc ^= (uint16_t)(*data++) << 8;
        for (uint8_t i = 0; i < 8; ++i)
        {
            if ( crc & 0x8000 )
            {
                crc = (crc << 1) ^ crc_poly;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-16/DNP          x16+x13+x12+x11+x10+x8+x6+x5+x2+1
 * Poly:    0x3D65
 * Init:    0x0000
 * Refin:   True
 * Refout:  True
 * Xorout:  0xFFFF
 * Use:     M-Bus,ect.
 *****************************************************************************/
uint16_t crc16_dnp(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0;            // Initial value
    while(length--)
    {
        crc ^= *data++;            // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA6BC;        // 0xA6BC = reverse 0x3D65
            else
                crc = (crc >> 1);
        }
    }
    return ~crc;                // crc^Xorout
}

/******************************************************************************
 * Name:    CRC-32  x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
 * Poly:    0x4C11DB7
 * Init:    0xFFFFFFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0xFFFFFFF
 * Alias:   CRC_32/ADCCP
 * Use:     WinRAR,ect.
 *****************************************************************************/
uint32_t crc32(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint32_t crc = 0xffffffff;        // Initial value
    while(length--)
    {
        crc ^= *data++;                // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;// 0xEDB88320= reverse 0x04C11DB7
            else
                crc = (crc >> 1);
        }
    }
    return ~crc;
}

/******************************************************************************
 * Name:    CRC-32/MPEG-2  x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
 * Poly:    0x4C11DB7
 * Init:    0xFFFFFFF
 * Refin:   False
 * Refout:  False
 * Xorout:  0x0000000
 * Note:
 *****************************************************************************/
uint32_t crc32_mpeg_2(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint32_t crc = 0xffffffff;  // Initial value
    while(length--)
    {
        crc ^= (uint32_t)(*data++) << 24;// crc ^=(uint32_t)(*data)<<24; data++;
        for (i = 0; i < 8; ++i)
        {
            if ( crc & 0x80000000 )
                crc = (crc << 1) ^ 0x04C11DB7;
            else
                crc <<= 1;
        }
    }
    return crc;
}
