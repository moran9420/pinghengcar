#include "zf_device_mpu6050.h"
#include <math.h>

// 算法参数：Kp控制加速度计补偿速度，Ki控制偏移补偿速度
#define Kp 2.0f      
#define Ki 0.001f    
#define halfT 0.0015f // 采样周期的一半。假设你在5ms(200Hz)中断里运行，dt=0.008，则halfT=0.004

float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f; // 四元数
float exInt = 0.0f, eyInt = 0.0f, ezInt = 0.0f;  // 误差积分项

// 最终解算出的角度（全局变量）
float Roll, Pitch, Yaw; 

void mahony_update(float gx, float gy, float gz, float ax, float ay, float az) {
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;

    // 1. 对加速度数据进行归一化
    norm = sqrtf(ax*ax + ay*ay + az*az);
    if(norm == 0.0f) return; 
    ax /= norm; ay /= norm; az /= norm;

    // 2. 提取当前四元数中的重力分量（估计重力方向）
    vx = 2 * (q1*q3 - q0*q2);
    vy = 2 * (q0*q1 + q2*q3);
    vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;

    // 3. 计算误差：测量值(a)与预测值(v)的向量积
    ex = (ay*vz - az*vy);
    ey = (az*vx - ax*vz);
    ez = (ax*vy - ay*vx);

    // 4. 对误差进行积分和比例补偿
    exInt += ex * Ki;
    eyInt += ey * Ki;
    ezInt += ez * Ki;
    gx += Kp * ex + exInt;
    gy += Kp * ey + eyInt;
    gz += Kp * ez + ezInt;

    // 5. 一阶龙格库塔法更新四元数
    q0 += (-q1*gx - q2*gy - q3*gz) * halfT;
    q1 += (q0*gx + q2*gz - q3*gy) * halfT;
    q2 += (q0*gy - q1*gz + q3*gx) * halfT;
    q3 += (q0*gz + q1*gy - q2*gx) * halfT;

    // 6. 四元数再次归一化
    norm = sqrtf(q0*q0 + q1*q1 + q2*q2 + q3*q3);
    q0 /= norm; q1 /= norm; q2 /= norm; q3 /= norm;

    // 7. 转换为欧拉角（平衡车通常只需要 Pitch 或 Roll）
    Pitch = asin(-2 * q1 * q3 + 2 * q0 * q2) * 57.29578f; // 弧度转角度
    Roll  = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * 57.29578f;
    Yaw   = atan2(2 * (q1 * q2 + q0 * q3), q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3) * 57.29578f;
}
