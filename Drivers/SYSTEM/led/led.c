#include "led.h"

void LED_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能GPIOE时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();
    
    // 配置PE5引脚
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;           // 引脚5
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // 推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;         // 无上下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // 低速输出
    
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    // 初始状态：熄灭LED（高电平）
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
}

