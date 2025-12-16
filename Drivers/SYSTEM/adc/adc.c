#include "adc.h"
#include "main.h"
#include "./SYSTEM/uart/uart.h"
#include "./SYSTEM/dma/dma.h"

ADC_HandleTypeDef hadc1;

volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];

void MY_ADC_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    
    // 关键：使用外部触发（TIM1 TRGO）
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO;
    
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = ENABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    
    HAL_ADC_Init(&hadc1);
    
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

void ADC_StartConversion(void)
{
    system_state.adc_conversion_complete = 0;
    
    // 启动 DMA 传输（仅一次）
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_BUFFER_SIZE) != HAL_OK)
    {
        UART_Printf("\nADC DMA start failed!\r\n");
        Error_Handler();
    }
		
		__HAL_DMA_ENABLE_IT(&hdma_adc1, DMA_IT_TC);
    UART_Printf("\nADC+DMA started, waiting for %d samples @ 10kHz...\r\n", ADC_BUFFER_SIZE);
}


// DMA传输完成回调函数 - 重要！
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1) {
        system_state.adc_conversion_complete = 1;
        UART_Printf("ADC complete! first value: %d\n", adc_buffer[0]);
    }else{
				UART_Printf("ADC Callback Error!\n");
		}
		
}
// 输出ADC电压值
void Print_ADC_Voltage_Values(void)
{
    if (!system_state.adc_conversion_complete) {
        UART_Printf("ADC data no sample，please wait\n");
        return;
    }

    UART_Printf("=== ADC (20) ===\n");
    for(int i = 0; i < 20 && i < ADC_BUFFER_SIZE; i++) {
        float voltage = (float)adc_buffer[i] * 3.3f / 4095.0f;
        UART_Printf("ADC[%3d] = %4d -> %.3f V\n", i, adc_buffer[i], voltage);
    }

    
    // 显示统计信息
    uint32_t sum = 0;
    uint16_t min_val = 4095, max_val = 0;
    
    for(int i = 0; i < ADC_BUFFER_SIZE; i++) {
        sum += adc_buffer[i];
        if(adc_buffer[i] < min_val) min_val = adc_buffer[i];
        if(adc_buffer[i] > max_val) max_val = adc_buffer[i];
    }
    
    float average = (float)sum / ADC_BUFFER_SIZE;
    float avg_voltage = average * 3.3f / 4095.0f;
    
    UART_Printf("\n Massage:\n");
    UART_Printf("  Min: %d (%.3f V)\n", min_val, min_val * 3.3f / 4095.0f);
    UART_Printf("  Max: %d (%.3f V)\n", max_val, max_val * 3.3f / 4095.0f);
    UART_Printf("  Avager: %.1f (%.3f V)\n", average, avg_voltage);
    
    // 重置标志以便下次采样
    system_state.adc_conversion_complete = 0;
}

