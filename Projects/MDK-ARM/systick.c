#include "stm32f1xx.h"
#include "systick.h"
#include "key.h"
#include "led.h"
// ����Ϊʲô,ע���ˣ����ﴫ�ݵĲ�����װ��ֵ����Ƶ��72M��װ��ֵ/��Ƶ���ڶ�ʱʱ��
void SysTick_Init(void)
{
    // ϵͳƵ��Ϊ 72MHz��SysTick ÿ���ж�һ��
    HAL_SYSTICK_Config(72000000 / 1000); // 72 MHz = 72,000,000 ����
}


void HAL_SYSTICK_Callback(void)
{
    Key_Scan();
}
