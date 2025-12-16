#ifndef __UART_H
#define __UART_H

#include "main.h"
#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef huart1;

void UART_Init(void);
void Send_ADC_Data_UART(void);
uint16_t CRC16_Modbus(const uint8_t *data, uint16_t len);
void UART_Printf(const char *format, ...);
void UART_OutputPeakFrequency(void);
void UART_OutputSpectrumData(void);

#endif




