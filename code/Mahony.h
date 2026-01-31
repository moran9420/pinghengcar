#ifndef _MAHONY_H_
#define _MAHONY_H_

#include "zf_common_typedef.h" // 引用逐飞库的数据类型定义

// --- 算法核心参数 ---
// 如果你的平衡车震动很大，尝试增大 Kp；如果姿态漂移缓慢，尝试增大 Ki
#define MAHONY_KP     2.0f      
#define MAHONY_KI     0.001f    

// --- 采样周期设置 ---
// 重要：halfT 必须等于 (1 / 采样频率) / 2
// 假设你在 PIT 定时器 5ms 中断里运行，频率是 200Hz，dt = 0.005s，则 halfT = 0.0025f
// 假设你在 PIT 定时器 10ms 中断里运行，频率是 100Hz，dt = 0.01s，则 halfT = 0.005f
#define MAHONY_HALFT  0.0015f   

// --- 全局变量声明 ---
// 外部文件（如 main.c 或 control.c）可以直接访问这些解析出的欧拉角
extern float Roll, Pitch, Yaw; 

// --- 函数声明 ---

/**
 * @brief Mahony 姿态解算更新函数
 * @param gx, gy, gz  陀螺仪数据 (单位: rad/s)
 * @param ax, ay, az  加速度计数据 (单位: 任意，内部会归一化)
 */
 void low_pass_filter(float *res, float raw, float alpha) ;
void mahony_update(float gx, float gy, float gz, float ax, float ay, float az);

/**
 * @brief 四元数复位（通常在初始化或车子摔倒重新扶起时调用）
 */
void mahony_reset(void);

#endif // _MAHONY_H_