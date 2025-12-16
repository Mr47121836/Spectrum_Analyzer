// fft.c
#include "fft.h"
#include "./SYSTEM/uart/uart.h"
#include "arm_math.h"
#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

#ifndef FFT_SIZE
#define FFT_SIZE 2048
#endif

extern volatile uint16_t adc_buffer[FFT_SIZE];
extern volatile float filtered_adc_buffer[FFT_SIZE]; 

// 复数输入：2 * 2048 = 4096 个 float32
__attribute__((section(".ccmram")))
static float32_t fft_input[FFT_SIZE * 2] __attribute__((aligned(8)));

// 幅度谱：2048/2 = 1024
__attribute__((section(".ccmram")))
static float32_t magnitude[FFT_SIZE/2 + 1] __attribute__((aligned(8)));

// 使用 Radix-2（支持任意 2 的幂，包括 2048）
arm_cfft_radix2_instance_f32 S;
float32_t max;
void FFT_Init(void)
{
    arm_status status = arm_cfft_radix2_init_f32(&S, FFT_SIZE, 0, 1);
    if (status != ARM_MATH_SUCCESS) {
        UART_Printf("FFT INIT FAILED: %d\r\n", status);
        Error_Handler();
    } else {
        UART_Printf("FFT INIT OK (2048-pt Radix-2)\r\n");
    }
}
void FFT_Process(void)
{
    // 填充实部 + 窗函数，虚部清零
    for (uint32_t i = 0; i < FFT_SIZE; i++)
    {
        float32_t sample = (adc_buffer[i] / 2047.5f) - 1.0f;
        float32_t window = 0.54f - 0.46f*arm_cos_f32(2.0f * PI * i / (FFT_SIZE - 1));
        fft_input[2*i]     = sample * window;
        fft_input[2*i + 1] = 0.0f;
    }

    // 执行 2048 点 Radix-2 FFT
    arm_cfft_radix2_f32(&S, fft_input);

    // 计算幅度谱
    arm_cmplx_mag_f32(fft_input, magnitude, FFT_SIZE/2);

}
void FFT_ProcessSpectrum(void)
{
    FFT_Process();

    /* ---------- 1. 忽略 DC ---------- */
    //magnitude[0] = 0.0f;

    /* ---------- 2. 找最大 bin ---------- */
    uint32_t peak_index = 0;
    float32_t peak_magnitude = 0.0f;
    arm_max_f32(&magnitude[0], FFT_SIZE/2 - 1, &peak_magnitude, &peak_index);
    //peak_index += 1;

    /* ---------- 3. 频率分辨率 ---------- */
    float32_t bin_width = (float32_t)system_state.sampling_frequency / FFT_SIZE;
    float32_t freq = peak_index * bin_width;

    /* ---------- 4. 抛物线插值 ---------- */
    if (peak_index > 1 && peak_index < FFT_SIZE/2 - 1)
    {
        float32_t y1 = magnitude[peak_index - 1];
        float32_t y2 = magnitude[peak_index];
        float32_t y3 = magnitude[peak_index + 1];
        float32_t denom = y1 - 2.0f * y2 + y3;
        if (denom != 0.0f)
        {
            float32_t delta = 0.5f * (y1 - y3) / denom;
            freq += delta * bin_width;
        }
    }

    /* ---------- 5. 计算 dB 值 ---------- */
    float32_t peak_dB = 20.0f * log10f(peak_magnitude + 1e-12f);

    /* ---------- 6. 更新全局状态 ---------- */
    system_state.peak_frequency = freq;
    system_state.peak_magnitude = peak_dB;

    // 存储dB谱
    for (int i = 0; i < FFT_SIZE / 2; i++)
    {
        system_state.magnitude[i] = 20.0f * log10f(magnitude[i] + 1e-12f);
    }

    /* ---------- 7. UART 打印结果 ---------- */
    UART_Printf("\r\n"
                "=== FFT Spectrum Result ===\r\n"
                "Peak Bin     : %lu\r\n"
                "Peak Magnitude : %.2f dB\r\n"
                "Peak Frequency : %.2f Hz\r\n"
                "Bin Width    : %.2f Hz\r\n"
                "Fs           : %lu Hz\r\n"
                "FFT Size     : %d\r\n"
                "==============================\r\n",
                (unsigned long)peak_index,
                (double)peak_dB,
                (double)freq,
                (double)bin_width,
                (unsigned long)system_state.sampling_frequency,
                FFT_SIZE);
}