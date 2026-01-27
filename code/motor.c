#include "key.h"
#include "zf_device_key.h"
#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_driver_gpio.h"
#include "zf_driver_timer.h"
#include "zf_driver_pwm.h"
#include "motor.h"
float targetspeed=0;
float targetangle=0;
float targetturn=0;
float actualspeed=0;
float actualangle=0;
float actualturn=0;




#define MOTOR_L_PWM_PIN    TIM5_PWM_CH2_A1


#define MOTOR_R_PWM_PIN    TIM5_PWM_CH4_A3

#define MOTOR_L_IN1_PIN    C0
#define MOTOR_L_IN2_PIN    C1

#define MOTOR_R_IN1_PIN    C2
#define MOTOR_R_IN2_PIN    C3

static void motor_set(int dir_pin, pwm_channel_enum pwm_pin, int speed)
{
    gpio_pin_enum in1, in2;

    /* 根据 dir_pin 判断是左电机还是右电机 */
    if(dir_pin == MOTOR_L_IN1_PIN)
    {
        in1 = MOTOR_L_IN1_PIN;
        in2 = MOTOR_L_IN2_PIN;
    }
    else    // 默认右电机
    {
        in1 = MOTOR_R_IN1_PIN;
        in2 = MOTOR_R_IN2_PIN;
    }

    if(speed > MOTOR_PWM_MAX)
        speed = MOTOR_PWM_MAX;
    if(speed < -MOTOR_PWM_MAX)
        speed = -MOTOR_PWM_MAX;

    if(speed > 0)
    {
        gpio_set_level(in1, GPIO_HIGH);
        gpio_set_level(in2, GPIO_LOW);
        pwm_set_duty(pwm_pin, speed);
    }
    else if(speed < 0)
    {
        gpio_set_level(in1, GPIO_LOW);
        gpio_set_level(in2, GPIO_HIGH);
        pwm_set_duty(pwm_pin, -speed);
    }
    else
    {
        // TB6612 主动刹车（平衡车必须）
        gpio_set_level(in1, GPIO_LOW);
        gpio_set_level(in2, GPIO_LOW);
        pwm_set_duty(pwm_pin, 0);
    }
}


void motor_init(void)
{
    gpio_init(MOTOR_L_IN1_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
    gpio_init(MOTOR_L_IN2_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);

    gpio_init(MOTOR_R_IN1_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
    gpio_init(MOTOR_R_IN2_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);

    pwm_init(MOTOR_L_PWM_PIN, MOTOR_PWM_FREQ, 0);
    pwm_init(MOTOR_R_PWM_PIN, MOTOR_PWM_FREQ, 0);
}

void motor_set_left(int speed)
{
    motor_set(MOTOR_L_IN1_PIN, MOTOR_L_PWM_PIN, speed);
}

void motor_set_right(int speed)
{
    motor_set(MOTOR_R_IN1_PIN, MOTOR_R_PWM_PIN, speed);
}

void motor_stop(void)
{
    pwm_set_duty(MOTOR_L_PWM_PIN, 0);
    pwm_set_duty(MOTOR_R_PWM_PIN, 0);
}
void Car_Start(void)
{
   
    if(targetangle > -2.0f)
    {
        targetangle -= 0.02f;
        if(targetangle < -2.0f)
            targetangle = -2.0f;
    }
}
void Car_Stop(void)
{	
    if(targetangle < 0.0f)
    {
        targetangle += 0.03f;
        if(targetangle > 0.0f)
            targetangle = 0.0f;
    }
}
void Car_back(void)
{
   
    if(targetangle <2.0f)
    {
        targetangle += 0.02f;
        if(targetangle >2.0f)
            targetangle = 2.0f;
    }
}