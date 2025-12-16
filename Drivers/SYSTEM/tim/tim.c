#include "tim.h"

TIM_HandleTypeDef htim2; // 改为全局变量

void TIM_Init(void)
{
    __HAL_RCC_TIM2_CLK_ENABLE();
    
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;                    // 168MHz 
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 81;                      // 168MHz / 8190 = 10kHz 采样率
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.RepetitionCounter = 0;
    HAL_TIM_Base_Init(&htim2);

    // 启用 Update Event 作为 TRGO
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);

    // 启动定时器
    HAL_TIM_Base_Start(&htim2);
		system_state.sampling_frequency =  168000000/(htim2.Init.Period+1)*(htim2.Init.Prescaler+1);
}

//采样率 = APB2_Timer_Clock / ((Prescaler+1) * (Period+1))


