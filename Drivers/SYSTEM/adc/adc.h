// adc.h
#ifndef __ADC_H
#define __ADC_H

#include "main.h"
#include "system_config.h" 

#define ADC_BUFFER_SIZE 2048

extern ADC_HandleTypeDef hadc1;
extern volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
extern volatile uint8_t adc_conversion_complete;

void MY_ADC_Init(void);
void ADC_StartConversion(void);
void Print_ADC_Voltage_Values(void);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
void Print_ADC_Voltage_Values(void);

#endif
