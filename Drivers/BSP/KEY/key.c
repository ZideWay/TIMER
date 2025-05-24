#include "key.h"
#include "led.h"
#include "key.h"

static KeyState key_state = KEY_RELEASE;
static uint16_t key_timer = 0;
static uint8_t key_short_flag = 0;
static uint8_t key_long_flag = 0;

void Key_Init(void)
{
    KEY_GPIO_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = KEY_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStruct);
}


static uint8_t Key_IsPressed(void)
{
    return HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_GPIO_PIN) == GPIO_PIN_RESET;
}

void Key_Scan(void)
{
    switch (key_state)
    {
        case KEY_RELEASE:
            if (Key_IsPressed())
            {
                key_state = KEY_DEBOUNCE;
                key_timer = 0;
            }
            break;

        case KEY_DEBOUNCE:
            if (Key_IsPressed())
            {
                if (++key_timer >= DEBOUNCE_TIME)
                {
                    key_state = KEY_SHORT_PRESS;
                    key_timer = 0;
                }
            }
            else
            {
                key_state = KEY_RELEASE;
            }
            break;

        case KEY_SHORT_PRESS:
            if (!Key_IsPressed())
            {
                key_short_flag = 1;
                key_state = KEY_RELEASE;
            }
            else if (++key_timer >= LONG_PRESS_TIME)
            {
                key_state = KEY_LONG_PRESS;
            }
            break;

        case KEY_LONG_PRESS:
            if (!Key_IsPressed())
            {
                key_long_flag = 1;
                key_state = KEY_RELEASE;
            }
            break;
    }
}

uint8_t Key_WasShortPressed(void)
{
    if (key_short_flag)
    {
        key_short_flag = 0;
        return 1;
    }
    return 0;
}

uint8_t Key_WasLongPressed(void)
{
    if (key_long_flag)
    {
        key_long_flag = 0;
        return 1;
    }
    return 0;
}