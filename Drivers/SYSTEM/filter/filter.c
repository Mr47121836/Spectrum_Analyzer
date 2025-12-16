#include "filter.h"
#include <string.h>

// 卡尔曼滤波函数实现
void KalmanFilter_Init(KalmanFilter* kf, float q, float r, float initial_x, float initial_p) {
    kf->q = q;
    kf->r = r;
    kf->x = initial_x;
    kf->p = initial_p;
    kf->k = 0.0f;
}

float KalmanFilter_Update(KalmanFilter* kf, float measurement) {
    // 预测更新
    kf->p = kf->p + kf->q;
    
    // 测量更新
    kf->k = kf->p / (kf->p + kf->r);
    kf->x = kf->x + kf->k * (measurement - kf->x);
    kf->p = (1 - kf->k) * kf->p;
    
    return kf->x;
}

float KalmanFilter_Predict(KalmanFilter* kf) {
    // 仅进行预测，不更新状态
    kf->p = kf->p + kf->q;
    return kf->x;
}

void KalmanFilter_Reset(KalmanFilter* kf, float new_x, float new_p) {
    kf->x = new_x;
    kf->p = new_p;
    kf->k = 0.0f;
}

void KalmanFilter_SetParameters(KalmanFilter* kf, float q, float r) {
    kf->q = q;
    kf->r = r;
}

float KalmanFilter_GetGain(KalmanFilter* kf) {
    return kf->k;
}

// 移动平均滤波函数实现
void MovingAverage_Init(MovingAverageFilter* filter) {
    memset(filter->buffer, 0, sizeof(filter->buffer));
    filter->index = 0;
    filter->count = 0;
    filter->sum = 0.0f;
}

float MovingAverage_Update(MovingAverageFilter* filter, float new_value) {
    // 减去最旧的值（如果缓冲区已满）
    if (filter->count == MOVING_AVERAGE_SIZE) {
        filter->sum -= filter->buffer[filter->index];
    } else {
        filter->count++;
    }
    
    // 添加新值
    filter->buffer[filter->index] = new_value;
    filter->sum += new_value;
    
    // 更新索引
    filter->index = (filter->index + 1) % MOVING_AVERAGE_SIZE;
    
    // 返回平均值
    return filter->sum / filter->count;
}

void MovingAverage_Reset(MovingAverageFilter* filter) {
    MovingAverage_Init(filter);
}

// 一阶低通滤波函数实现
void LowPassFilter_Init(LowPassFilter* filter, float alpha, float initial_value) {
    filter->alpha = alpha;
    filter->output = initial_value;
}

float LowPassFilter_Update(LowPassFilter* filter, float new_value) {
    filter->output = filter->alpha * new_value + (1 - filter->alpha) * filter->output;
    return filter->output;
}

void LowPassFilter_SetAlpha(LowPassFilter* filter, float alpha) {
    filter->alpha = alpha;
}