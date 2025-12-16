#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "system_config.h"  // 包含系统配置

// 注释掉这行使用本地FFT实现
// #define USE_CMSIS_DSP

// 错误处理
void Error_Handler(void);

// 系统时钟
void SystemClock_Config(void);


// 重定向printf到串口
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

#ifdef __cplusplus
}
#endif

#endif


