#include "pwm.h"

TIM_HandleTypeDef htim1;

void TIM1_PWM_Init(void)
{
    TIM_OC_InitTypeDef sConfigOC = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 1. 使能时钟
    __HAL_RCC_TIM1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 2. 配置 PA8 为 TIM1_CH1 的复用功能 (AF1 for TIM1)
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;           // 复用推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;        // TIM1_CH1 复用功能
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 3. 配置 TIM1 基础参数
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 84 - 1;       //84MHz / 84   = 1MHz		
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 1000  - 1;       // 1MHz / 1000 = 1KHz                 
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(&htim1);                         // 初始化为 PWM 模式

    // 4. 配置 PWM 通道 1 (CH1)
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 500-1;                           
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    // 5. 启动 PWM 输出
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

    // 可选：使能主输出 (高级定时器需要)
    __HAL_TIM_MOE_ENABLE(&htim1);
}