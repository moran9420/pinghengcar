#include "mm32_device.h"                // Device header
#include "angle.h"
#include "zf_device_mpu6050.h"
#include <math.h>

float CarAngle = 0;

static int16_t ax, ay, az, gx, gy, gz;

#define GYRO_OFFSET      16
#define ANGLE_ACC_OFFSET 0.5f
#define DT               0.01f
#define ALPHA            0.01f

void Angle_Init(void)
{
    CarAngle = 0;
}

void Angle_Update(void)
{
    mpu6050_get_acc();
    mpu6050_get_gyro();
	ax=mpu6050_acc_x;
	ay=mpu6050_acc_y;
	az=mpu6050_acc_z;
	gx=mpu6050_gyro_x;
	gy=mpu6050_gyro_y;
	gz=mpu6050_gyro_z;
    gy -= GYRO_OFFSET;

    float angle_acc = -atan2((float)ax, (float)az) * 57.29578f;
    angle_acc += ANGLE_ACC_OFFSET;

    float angle_gyro = CarAngle + gy / 32768.0f * 2000.0f * DT;

    CarAngle = ALPHA * angle_acc + (1.0f - ALPHA) * angle_gyro;
}
