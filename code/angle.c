#include "angle.h"
#include "zf_device_mpu6050.h"
#include "Mahony.h"
#include <math.h>

float CarAngle = 0;

// 度转弧度系数
#define D2R 0.0174533f

void Angle_Init(void)
{
    // 这里可以加入陀螺仪静态校准逻辑
    CarAngle = 0;
}

void Angle_Update(void)
{
    // 1. 读取 MPU6050 原始数据
    mpu6050_get_acc();
    mpu6050_get_gyro();

    // 2. 将陀螺仪原始数据转换为 弧度/秒 (rad/s)
    // 假设量程为 +-2000dps, 对应的灵敏度通常是 16.4 LSB/(deg/s)
    // 根据逐飞库，建议使用其转换函数，如果没有现成的，手动计算如下：
    float gx_rad = (float)mpu6050_gyro_x / 16.4f * D2R;
    float gy_rad = (float)mpu6050_gyro_y / 16.4f * D2R;
    float gz_rad = (float)mpu6050_gyro_z / 16.4f * D2R;

    // 3. 将加速度计数据转换为浮点数（Mahony 内部会处理归一化，所以直接传入原始数值即可）
    float ax_f = (float)mpu6050_acc_x;
    float ay_f = (float)mpu6050_acc_y;
    float az_f = (float)mpu6050_acc_z;

    // 4. 调用 Mahony 算法更新四元数并计算欧拉角
    // 注意：根据你传感器的安装方向，可能需要调换 ax/ay 和 gx/gy 的位置
    mahony_update(gx_rad, gy_rad, gz_rad, ax_f, ay_f, az_f);

    // 5. 赋值给平衡控制使用的 CarAngle
    // 如果你的小车是纵向放置，可能用 Pitch；横向放置则用 Roll
    CarAngle = Pitch; 
}