// fft_processor.h
#ifndef __FFT_PROCESSOR_H
#define __FFT_PROCESSOR_H

#include "main.h"
#include "arm_math.h"
#include "system_config.h"  // 包含系统配置

extern float32_t fft_input[2*FFT_SIZE];
extern float32_t fft_output[FFT_SIZE];
extern float32_t magnitude[FFT_SIZE/2+1];

void FFT_Init(void);
void FFT_Process(void);
void FFT_ProcessSpectrum(void);

#endif



