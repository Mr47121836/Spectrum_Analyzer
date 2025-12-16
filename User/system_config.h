#ifndef __SYSTEM_CONFIG_H
#define __SYSTEM_CONFIG_H

#include "main.h"

#define FFT_SIZE 2048
#define PI 3.14159265358979323846f

// 系统状态结构体
typedef struct {
    float sampling_frequency;
    uint32_t fft_size;
    uint8_t spectrum_ready;
		uint8_t adc_conversion_complete;
    float peak_frequency;
    float peak_magnitude;
		float peak_magnitude_linear;
    float magnitude[FFT_SIZE/2];
} SystemState;

// 声明全局系统状态变量
extern SystemState system_state;

#endif

