#include "usart_module.h"
#include <string.h>


uint16_t Calculate_CRC16(const uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

uint16_t Build_UART_Frame(uint16_t func_code, uint8_t *data, uint16_t data_len, uint8_t *tx_buf)
{
    // 1. ���֡ͷ
    tx_buf[0] = 0x5A;
    tx_buf[1] = 0xA5;
	
	// 2. ��书����
    tx_buf[2] = func_code & 0xFF;
    tx_buf[3] = (func_code >> 8) & 0xFF;
	
    // 3. ����֡���ȣ�֡ͷ2 + ����2 + ������2 + ����N + CRC2 + ֡β2��
    uint16_t frame_length = 2 + 2 + 2 + data_len + 2 + 2;
    tx_buf[4] = frame_length & 0xFF;       // ���ȵ��ֽ�
    tx_buf[5] = (frame_length >> 8) & 0xFF; // ���ȸ��ֽ�



    // 4. �������
    memcpy(&tx_buf[6], data, data_len);

    // 5. ����CRC����֡ͷ�����ݲ��֣�
    uint16_t crc = Calculate_CRC16(tx_buf, 6 + data_len);
    tx_buf[6 + data_len] = crc & 0xFF;     // CRC���ֽ�
    tx_buf[7 + data_len] = (crc >> 8) & 0xFF; // CRC���ֽ�

    // 6. ���֡β��������
    tx_buf[8 + data_len] = FRAME_TAIL_1;   // 0x69
    tx_buf[9 + data_len] = FRAME_TAIL_2;   // 0x69

    return frame_length;  // ������֡��
}

bool Parse_UART_Frame(uint8_t *rx_buf, uint16_t rx_len)
{
    if (rx_len < 10)  // header2 + func_code2 + length2 + crc2 + tail2
        return false;

    // 1. ֡ͷ���
    if (rx_buf[0] != FRAME_HEADER_1 || rx_buf[1] != FRAME_HEADER_2)
        return false;

    // 2. ��ȡ�������֡����
    uint16_t func_code = (rx_buf[2]) | (rx_buf[3] << 8);        // С��
    uint16_t frame_length = (rx_buf[4]) | (rx_buf[5] << 8);     // С��

    if (frame_length != rx_len)
        return false;

    // 3. ֡β���
    if (rx_buf[rx_len - 2] != FRAME_TAIL_1 || rx_buf[rx_len - 1] != FRAME_TAIL_2)
        return false;

    // 4. CRC У��
    uint16_t crc_received = (rx_buf[rx_len - 4]) | (rx_buf[rx_len - 3] << 8);  // С��
    uint16_t crc_calculated = Calculate_CRC16(rx_buf, rx_len - 4);
    if (crc_calculated != crc_received)
        return false;

    // 5. ��������ṹ��
    UART_Frame_t frame;
    memset(&frame, 0, sizeof(UART_Frame_t));
    frame.header[0] = rx_buf[0];
    frame.header[1] = rx_buf[1];
    frame.func_code = func_code;
    frame.length = frame_length;

    uint16_t data_len = frame_length - 10;
    if (data_len > sizeof(frame.data))
        data_len = sizeof(frame.data);
    memcpy(frame.data, &rx_buf[6], data_len);

    frame.crc = crc_received;
    frame.tailer[0] = rx_buf[rx_len - 2];
    frame.tailer[1] = rx_buf[rx_len - 1];

    // 6. ���ô�����
    extern int dispatch_command(UART_Frame_t *recv_frame, uint16_t frame_len);
    dispatch_command(&frame, frame.length);

    return true;
}


