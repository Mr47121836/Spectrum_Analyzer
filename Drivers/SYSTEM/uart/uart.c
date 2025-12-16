// uart_output.c
#include "system_config.h"
#include "uart.h"
#include <stdio.h>
#include <stdarg.h>
#include "./SYSTEM/adc/adc.h"


UART_HandleTypeDef huart1;

void UART_Init(void)
{
    // 使能时钟
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // 配置GPIO
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;  
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 配置UART参数
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    
    // 初始化UART
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        //HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
				//HAL_Delay(1000);
    }
    
    // 注意：只有需要中断接收时才启用中断
    // HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    // HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void UART_Printf(const char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, format);
    int length = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    // 安全检查
    if (length < 0) {
        return; // 格式化错误
    }
    else if (length >= sizeof(buffer)) {
        length = sizeof(buffer) - 1; // 截断
    }
    
    if(length > 0)
    {
        HAL_UART_Transmit(&huart1, (uint8_t*)buffer, length, HAL_MAX_DELAY);

    }
}

void UART_OutputPeakFrequency(void)
{
    // 添加空指针检查
    if (system_state.peak_frequency >= 0) {
        UART_Printf("Peak: %.2f Hz, Magnitude: %.4f\r\n", 
                    system_state.peak_frequency, 
                    system_state.peak_magnitude);
    }
}

void UART_OutputSpectrumData(void)
{
    UART_Printf("=== Spectrum Data ===\r\n");
    for(int i = 0; i < 10; i++) // 只输出前10个频点
    {
        float freq = (float)i * system_state.sampling_frequency / FFT_SIZE;
        UART_Printf("Freq: %.1f Hz, Mag: %.4f\r\n", freq, system_state.magnitude[i]);
    }
    UART_Printf("=====================\r\n");
}

// 如果使用中断，需要实现中断处理函数
/*
void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart2);
}
*/

uint16_t CRC16_Modbus(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

void Send_ADC_Data_UART(void)
{
    uint8_t tx_buf[4101];
    int idx = 0;

    // 1. SOF
    tx_buf[idx++] = 0xAA;

    // 2. LEN
    tx_buf[idx++] = 0x00;  // 4096 & 0xFF
    tx_buf[idx++] = 0x10;  // 4096 >> 8

    // 3. DATA (小端)
    for (int i = 0; i < ADC_BUFFER_SIZE; i++)
    {
        tx_buf[idx++] = adc_buffer[i] & 0xFF;
        tx_buf[idx++] = (adc_buffer[i] >> 8) & 0xFF;
    }

    // 4. CRC16 (Modbus)
    uint16_t crc = CRC16_Modbus(&tx_buf[3], 4096);  // 从 DATA 开始
    tx_buf[idx++] = crc & 0xFF;
    tx_buf[idx++] = (crc >> 8) & 0xFF;

    // 5. 发送
    HAL_UART_Transmit(&huart1, tx_buf, idx, HAL_MAX_DELAY);
    UART_Printf("Sent %d bytes, CRC=0x%04X\r\n", idx, crc);
}

