#include "stm32f1xx.h"
#include "systick.h"
#include "key.h"
#include "led.h"
// 这里为什么,注意了，这里传递的参数是装载值，主频是72M，装载值/主频等于定时时间
void SysTick_Init(void)
{
    // 系统频率为 72MHz，SysTick 每秒中断一次
    HAL_SYSTICK_Config(72000000 / 1000); // 72 MHz = 72,000,000 周期
}


void HAL_SYSTICK_Callback(void)
{
    Key_Scan();
}
