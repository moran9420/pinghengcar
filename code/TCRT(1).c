#include <stdint.h>
#include "zf_driver_pwm.h"
#include "zf_driver_gpio.h"
#include "zf_device_mpu6050.h"
#include "BEEP.h"

#include "TCRT(1).h"

static volatile uint8_t R2_raw = 0;
static volatile uint8_t R1_raw = 0;
static volatile uint8_t L1_raw = 0;
static volatile uint8_t L2_raw = 0;
static volatile uint8_t tcrt_snapshot = 0; // bit0=L2(E8), bit1=L1(E9), bit2=R1(E11), bit3=R2(E10)

//红外循迹模块初始化
void TCRT_Init(void)
{
    gpio_init(E8,  GPI, 0, GPI_PULL_UP);  //9 8 10 11
    gpio_init(E9,  GPI, 0, GPI_PULL_UP);
    gpio_init(E10, GPI, 0, GPI_PULL_UP);
    gpio_init(E11, GPI, 0, GPI_PULL_UP);

}

/*归一化，保证返回值属于-180~+180*/
float error(float a,float b)
{ 
	float error;
	error = a - b;
	if(-180 < error&&error < 180.0){error = error;}
	if(error >= 180){error = error - 360;}
	if(error <=-180){error = error + 360;}
	return error;
}

volatile float target_yaw;//陀螺仪目标角度
volatile float yaw;//当前偏航角

uint8_t goway=0;//角度循迹标志

/*pidturn角度循迹，定义上电时候的方向为targetangle则实现pid闭环控制保持直行，goway置1则使用角度循迹*/
void Turn_trace(void)
{
	if( goway == 1)
		{
			turn_error=error(target_yaw,yaw);
			turnpid.jifen+=turn_error;
			if(turnpid.jifen>turnpid.jifenxianzhi)turnpid.jifen=turnpid.jifenxianzhi;
			if(turnpid.jifen<-turnpid.jifenxianzhi)turnpid.jifen=-turnpid.jifenxianzhi;
			pricepwm=turn_error*turnpid.kp+(turn_error	- turnpid.lasterror)*turnpid.kd+turnpid.jifen*turnpid.ki;
			motor_speed(1500-pricepwm,1500+pricepwm);
			turnpid.lasterror=turn_error;
		}
	
}

/*红外循迹模块采样函数放到中断里定时读取红外电平原始值*/
void TCRT_Update(void)  
{
    uint8_t s = 0;
    s |= (gpio_get_level(E8)  ? (1<<0) : 0); // L2 -> bit0
    s |= (gpio_get_level(E9)  ? (1<<1) : 0); // L1 -> bit1  把 L1 的状态放第 1 位（0x02）
    s |= (gpio_get_level(E11) ? (1<<2) : 0); // R1 -> bit2
    s |= (gpio_get_level(E10) ? (1<<3) : 0); // R2 -> bit3

	L2_raw = (s & (1<<0)) ? 1 : 0;
    L1_raw = (s & (1<<1)) ? 1 : 0;
    R1_raw = (s & (1<<2)) ? 1 : 0;
    R2_raw = (s & (1<<3)) ? 1 : 0;
	
    tcrt_snapshot = s; 
}

static volatile uint8_t use_trace = 0;        // 1=允许TCRT_Trace更新target_angle

void TCRT_Trace(void)
{
	if(!use_trace) return;//关闭红外循迹
	
    float error_weight = 0;
    uint8_t cnt = 0;

    if(L2_raw) { error_weight -= 3; cnt++; }
    if(L1_raw) { error_weight -= 1; cnt++; }
    if(R1_raw) { error_weight += 1; cnt++; }
    if(R2_raw) { error_weight += 3; cnt++; }

	 if (cnt > 0) {
        target_angle = yaw + (float)error_weight * 20.0f; // 把相对量加到当前朝向
    }
	else {	 }//保持上一次
    while (target_angle > 180.0f)  target_angle -= 360.0f;
    while (target_angle <= -180.0f) target_angle += 360.0f;
}

#define Black_CNT        30
#define White_CNT        500
#define MODE3_TARGET     8

uint8_t Mode;//extern
static int8_t S2 = 0;//状态机2状态

/*模式2*/
void Mode2_Switch(void)
{
	goway=1;//角度循迹
    static uint32_t cnt = 0;
    if(S2 == 0)
    {
        if(R1_raw||R2_raw||L1_raw||L2_raw)
        {
			cnt++;
            if(cnt > Black_CNT)
            {
                Beep_Once();
                S2 = 1;
                cnt = 0;
				use_trace=1;

            }
        } else cnt = 0;
    }
    else if(S2 == 1)
    {
		TCRT_Trace();
        if(!(R1_raw||R2_raw||L1_raw||L2_raw))
        {
			cnt++;
            if(cnt > White_CNT)
            {
                Beep_Once();
                S2 = 2;
                cnt = 0;
				use_trace=0;
            }
        } else cnt = 0;
    }
    else if(S2 == 2)
    {
		target_angle=-180;
		goway=1;
        if(R1_raw||R2_raw||L1_raw||L2_raw)
        {
			cnt++;
            if(cnt > Black_CNT)
            {
                Beep_Once();
                S2 = 3;
                cnt = 0;
				use_trace=1;
           }
        } else cnt = 0;
    }
    else if(S2 == 3)
    {
		TCRT_Trace();
        if(!(R1_raw||R2_raw||L1_raw||L2_raw))
        {
            if(++cnt > White_CNT)
            {
                Stop;  //motor_speed( , )
                Beep_Once();
                S2 = 4;
				use_trace=0;
            }
        } else cnt = 0;
    }
}

static int8_t  S3 = 0;
volatile uint8_t mode3_laps=0;
/* 模式3*/
void Mode3_Switch(void)
{
	goway=1;
    static uint32_t cnt = 0;

    if(S3 == 0)
    {	
		
        if(R1_raw||R2_raw||L1_raw||L2_raw)
        {
			cnt++;
            if(cnt > Black_CNT)
            {
                Beep_Once();
                S3 = 1;
                cnt = 0;
				use_trace=1;
            }
        } else cnt = 0;
    }
    else if(S3 == 1)
    {
		TCRT_Trace();
        if(!(R1_raw||R2_raw||L1_raw||L2_raw))
        {
			cnt++;
            if(cnt > White_CNT)
            {
                Beep_Once();
                S3 = 2;
                cnt = 0;
				use_trace=0;
            }
        } else cnt = 0;
    }
    else if(S3 == 2)
		{		Car_Stop();//状态机加停止状态
        target_angle=+90;
		goway=1;
        if(R1_raw||R2_raw||L1_raw||L2_raw)
        {
			cnt++;
            if(cnt > Black_CNT)
            {
                Beep_Once();
                S3 = 3;
                cnt = 0;
				use_trace=1;
            }
        } else cnt = 0;
    }
	else if(S3 == 3)
    {
		TCRT_Trace();
        if(!(R1_raw||R2_raw||L1_raw||L2_raw))
        {
			cnt++;
            if(cnt > White_CNT)
            {
                Beep_Once();
                cnt = 0;
				use_trace=0;
				mode3_laps++;
				 if (mode3_laps >= 4)
				{
                    S3 = 4; // 进入停车子状态
                } else {
                    S3 = 0; // 启动下一圈
                }
            }
        } else cnt = 0;
    }
	else if(S3 == 4)  //停车子状态
	{
		stop;//motor_speed( , )
	}
}
