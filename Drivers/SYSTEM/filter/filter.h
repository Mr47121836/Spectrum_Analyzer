#ifndef __FILTER_H
#define __FILTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    float q; // 过程噪声协方差 - 系统模型的不确定性
    float r; // 测量噪声协方差 - 传感器测量的不确定性  
    float x; // 估计值 - 经过滤波后的最优估计
    float p; // 估计误差协方差 - 当前估计的可信度
    float k; // 卡尔曼增益 - 权衡预测和测量的权重
} KalmanFilter;

/**
  * @brief  卡尔曼滤波器初始化
  * @param  kf: 卡尔曼滤波器结构体指针
  * @param  q: 过程噪声协方差
  * @param  r: 测量噪声协方差
  * @param  initial_x: 初始估计值
  * @param  initial_p: 初始估计误差协方差
  * @retval 无
  */
void KalmanFilter_Init(KalmanFilter* kf, float q, float r, float initial_x, float initial_p);

/**
  * @brief  卡尔曼滤波更新
  * @param  kf: 卡尔曼滤波器结构体指针
  * @param  measurement: 新的测量值
  * @retval 滤波后的估计值
  */
float KalmanFilter_Update(KalmanFilter* kf, float measurement);

/**
  * @brief  卡尔曼滤波预测（仅预测，不更新）
  * @param  kf: 卡尔曼滤波器结构体指针
  * @retval 预测值
  */
float KalmanFilter_Predict(KalmanFilter* kf);

/**
  * @brief  重置卡尔曼滤波器状态
  * @param  kf: 卡尔曼滤波器结构体指针
  * @param  new_x: 新的初始估计值
  * @param  new_p: 新的初始估计误差协方差
  * @retval 无
  */
void KalmanFilter_Reset(KalmanFilter* kf, float new_x, float new_p);

/**
  * @brief  设置卡尔曼滤波器参数
  * @param  kf: 卡尔曼滤波器结构体指针
  * @param  q: 新的过程噪声协方差
  * @param  r: 新的测量噪声协方差
  * @retval 无
  */
void KalmanFilter_SetParameters(KalmanFilter* kf, float q, float r);

/**
  * @brief  获取当前卡尔曼增益
  * @param  kf: 卡尔曼滤波器结构体指针
  * @retval 当前卡尔曼增益值
  */
float KalmanFilter_GetGain(KalmanFilter* kf);

// 移动平均滤波相关定义
#define MOVING_AVERAGE_SIZE 16

typedef struct {
    float buffer[MOVING_AVERAGE_SIZE];
    uint8_t index;
    uint8_t count;
    float sum;
} MovingAverageFilter;

/**
  * @brief  移动平均滤波器初始化
  * @param  filter: 移动平均滤波器结构体指针
  * @retval 无
  */
void MovingAverage_Init(MovingAverageFilter* filter);

/**
  * @brief  移动平均滤波更新
  * @param  filter: 移动平均滤波器结构体指针
  * @param  new_value: 新的测量值
  * @retval 滤波后的值
  */
float MovingAverage_Update(MovingAverageFilter* filter, float new_value);

/**
  * @brief  重置移动平均滤波器
  * @param  filter: 移动平均滤波器结构体指针
  * @retval 无
  */
void MovingAverage_Reset(MovingAverageFilter* filter);

// 一阶低通滤波相关定义
typedef struct {
    float alpha;    // 滤波系数 (0-1)
    float output;   // 滤波输出
} LowPassFilter;

/**
  * @brief  一阶低通滤波器初始化
  * @param  filter: 低通滤波器结构体指针
  * @param  alpha: 滤波系数
  * @param  initial_value: 初始值
  * @retval 无
  */
void LowPassFilter_Init(LowPassFilter* filter, float alpha, float initial_value);

/**
  * @brief  一阶低通滤波更新
  * @param  filter: 低通滤波器结构体指针
  * @param  new_value: 新的测量值
  * @retval 滤波后的值
  */
float LowPassFilter_Update(LowPassFilter* filter, float new_value);

/**
  * @brief  设置低通滤波器参数
  * @param  filter: 低通滤波器结构体指针
  * @param  alpha: 新的滤波系数
  * @retval 无
  */
void LowPassFilter_SetAlpha(LowPassFilter* filter, float alpha);

#ifdef __cplusplus
}
#endif

#endif /* __FILTER_H */