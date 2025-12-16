#include "main.h"
#include "./SYSTEM/adc/adc.h"
#include "./SYSTEM/dma/dma.h"
#include "./SYSTEM/fft/fft.h"
#include "./SYSTEM/display/display.h"
#include "./SYSTEM/uart/uart.h"
#include "./SYSTEM/led/led.h"
#include "./SYSTEM/adc/adc.h"
#include "./SYSTEM/tim/tim.h"
#include "./SYSTEM/filter/filter.h"


// 定义全局系统状态变量
SystemState system_state = {0};

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    

    DMA_Init();
		MY_ADC_Init();
    TIM_Init();           // 启动定时器触发
    LED_GPIO_Init();
    UART_Init();
    FFT_Init();

    system_state.fft_size = FFT_SIZE;
    
    UART_Printf("STM32F407 Oscilloscope Ready!\r\n");
    
    // 启动 ADC+DMA（只启动一次）
    ADC_StartConversion();
    
    while (1)
    {
        if (system_state.adc_conversion_complete)
        {
            UART_Printf("ADC 	complete! Processing %d samples...\r\n", ADC_BUFFER_SIZE);
            
						//Print_ADC_Voltage_Values();
						Send_ADC_Data_UART();
					
						/* 2. 执行 FFT 并打印峰值频率 */
            FFT_ProcessSpectrum();  // ← 关键！

            /* 3. LED 闪烁 */
            HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);

            /* 4. 重启下一次采样 */
            system_state.adc_conversion_complete = 0;
            ADC_StartConversion();
        }
        
        HAL_Delay(10);
    }
}
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
        // 可以加 LED 闪烁提示错误
        HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_6);
        HAL_Delay(200);
    }
}
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    
    // 配置HSE
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
    
    // 配置时钟
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

/*

PLL输入: 8MHz / PLLM(8) = 1MHz
PLL倍频: 1MHz × PLLN(336) = 336MHz
系统时钟: 336MHz / PLLP(2) = 168MHz
AHB总线: 168MHz
APB1总线: 168MHz / 4 = 42MHz
APB2总线: 168MHz / 2 = 84MHz

*/

