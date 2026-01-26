#include <stdint.h>
#include "zf_driver_gpio.h"
#include "TCRT.h"
#include "motor.h"

static volatile uint8_t R2_raw = 0;
static volatile uint8_t R1_raw = 0;
static volatile uint8_t L1_raw = 0;
static volatile uint8_t L2_raw = 0;
uint16_t mode2flag=0;
//红外循迹模块初始化
void TCRT_Init(void)
{
    gpio_init(E8,  GPI, 0, GPI_PULL_UP);  //9 8 10 11
    gpio_init(E9,  GPI, 0, GPI_PULL_UP);
    gpio_init(E10, GPI, 0, GPI_PULL_UP);
    gpio_init(E11, GPI, 0, GPI_PULL_UP);

    R1_raw = R2_raw = L1_raw = L2_raw = 0;
}

//红外循迹模块采样函数
//放到中断里定时读取红外电平原始值
void TCRT_Update(void)  
{
    R2_raw = gpio_get_level(E10) ? 1 : 0;   //  1 2 2 1
    R1_raw = gpio_get_level(E11) ? 1 : 0;
    L1_raw = gpio_get_level(E9)  ? 1 : 0;
    L2_raw = gpio_get_level(E8)  ? 1 : 0;
}

static int8_t last_error = 0;

void TCRT_Trace(void)
{

    int8_t error = 0;
    uint8_t cnt = 0;

    if(L2_raw) { error -= 3; cnt++; }
    if(L1_raw) { error -= 1; cnt++; }
    if(R1_raw) { error += 1; cnt++; }
    if(R2_raw) { error += 3; cnt++; }

    /* 全白保护 */
//if(cnt == 0)
   // {
   //     error = last_error;        // 全白：保持上一次
   // }
   // else
  //  {
  //      last_error = error;
  //  }

//    targetturn = error;   // 给 turn PID
	  targetturn = error * 500;   // 放大，给转向环
}

#define Black_CNT        3
#define White_CNT        50
#define MODE3_TARGET     8

uint8_t Mode;
static int8_t S2 = 0;//状态机2状态

/*模式2*/
void Mode2_Switch(void)
{
    static uint32_t cnt = 0;

    if(S2 == 0)
    {	static uint16_t straghtflag1=1;
		if(straghtflag1==1)
		{
		targetturn=0;
			straghtflag1=0;
		}
	
		
        if(R1_raw||R2_raw||L1_raw||L2_raw)
        {
			cnt++;
            if(cnt > Black_CNT)
            {
                beep_start(200);
                S2 = 1;
                cnt = 0;
            }
        } else cnt = 0;
    }
    else if(S2 == 1)
    {
		
        if(!(R1_raw||R2_raw||L1_raw||L2_raw))
        {
			cnt++;
            if(cnt > White_CNT)
            {
                 beep_start(200);
                S2 = 2;
                cnt = 0;
            }
        } else cnt = 0;
    }
    else if(S2 == 2)
    {	static uint16_t straghtflag=1;
		if(straghtflag==1)
		{
		targetturn=0;
			straghtflag=0;
		}
        if(R1_raw||R2_raw||L1_raw||L2_raw)
        {
			cnt++;
            if(cnt > Black_CNT)
            {
                 beep_start(200);
                S2 = 3;
                cnt = 0;
            }
        } else cnt = 0;
    }
    else if(S2 == 3)
    {
        if(!(R1_raw||R2_raw||L1_raw||L2_raw))
        {cnt++;
            if(cnt > White_CNT)
            {
				Car_Stop();
                 beep_start(200);
                S2 = 4;
				mode2flag=0;
            }
        } else cnt = 0;
    }
}


static int8_t  S3 = 0;
static int16_t mode3_laps = 0;

/* 模式3*/
void Mode3_Switch(void)
{
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
            }
        } else cnt = 0;
    }
    else if(S3 == 1)
    {
        if(!(R1_raw||R2_raw||L1_raw||L2_raw))
        {
			cnt++;
            if(cnt > White_CNT)
            {
                Beep_Once();
                S3 = 10;
                cnt = 0;
            }
        } else cnt = 0;
    }
    else if(S3 == 10)//转向子状态
    {		
       
        mode3_laps++;

        if(mode3_laps >= MODE3_TARGET)
        {
           Car_Stop();
			last_error = 0;
            S3 = 4;
        }
        else
        {
            S3 = 0;
        }
    }
}


