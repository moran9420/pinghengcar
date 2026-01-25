#ifndef MOTOR_H
#define MOTOR_H
#include <stdint.h>
extern int16_t targetspeed1;
extern int16_t targetspeed2;
extern int16_t targetangle;
#include "zf_driver_pwm.h"
#include "zf_driver_gpio.h"

// ================= 参数配置 =================
#define MOTOR_PWM_FREQ     17000          // PWM 频率
#define MOTOR_PWM_MAX      PWM_DUTY_MAX   // 逐飞库最大占空比

// ================= 接口函数 =================
void motor_init(void);

// 设置左右电机速度
// speed 范围：[-MOTOR_PWM_MAX , MOTOR_PWM_MAX]
// 正数：正转，负数：反转
void motor_set_left(int speed);
void motor_set_right(int speed);

// 停车
void motor_stop(void);

#endif
