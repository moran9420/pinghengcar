#include "key.h"
#include "zf_device_key.h"
#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_driver_gpio.h"
#include "zf_driver_timer.h"
#include "zf_driver_pwm.h"
#include "motor.h"
int16_t targetspeed1=0;
int16_t targetspeed2=0;
int16_t targetangle=0;


#define MOTOR_L_DIR_PIN    A0
#define MOTOR_L_PWM_PIN    TIM5_PWM_CH2_A1


#define MOTOR_R_DIR_PIN    A2
#define MOTOR_R_PWM_PIN    TIM5_PWM_CH4_A3


static void motor_set(int dir_pin, pwm_channel_enum pwm_pin, int speed)
{
    if(speed > MOTOR_PWM_MAX)
        speed = MOTOR_PWM_MAX;
    if(speed < -MOTOR_PWM_MAX)
        speed = -MOTOR_PWM_MAX;

    if(speed >= 0)
    {
        gpio_set_level(dir_pin, GPIO_LOW);     // 正转
        pwm_set_duty(pwm_pin, speed);
    }
    else
    {
        gpio_set_level(dir_pin, GPIO_HIGH);    // 反转
        pwm_set_duty(pwm_pin, -speed);
    }
}


void motor_init(void)
{

    gpio_init(MOTOR_L_DIR_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
    gpio_init(MOTOR_R_DIR_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);

    pwm_init(MOTOR_L_PWM_PIN, MOTOR_PWM_FREQ, 0);
    pwm_init(MOTOR_R_PWM_PIN, MOTOR_PWM_FREQ, 0);
}

void motor_set_left(int speed)
{
    motor_set(MOTOR_L_DIR_PIN, MOTOR_L_PWM_PIN, speed);
}

void motor_set_right(int speed)
{
    motor_set(MOTOR_R_DIR_PIN, MOTOR_R_PWM_PIN, speed);
}

void motor_stop(void)
{
    pwm_set_duty(MOTOR_L_PWM_PIN, 0);
    pwm_set_duty(MOTOR_R_PWM_PIN, 0);
}
