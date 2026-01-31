#include <stdint.h>
#include "zf_driver_gpio.h"
#include "TCRT.h"
#include "motor.h"
#include "turn.h"
#include "math.h"

static volatile uint8_t R2_raw = 0;
static volatile uint8_t R1_raw = 0;
static volatile uint8_t L1_raw = 0;
static volatile uint8_t L2_raw = 0;
uint16_t mode2flag=0;
uint16_t mode3flag=0;
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
    {	targetspeed=4000;
		static uint16_t straghtflag1=1;
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
				targetspeed=0;
                 beep_start(200);
                S2 = 4;
				mode2flag=0;
            }
        } else cnt = 0;
    }
}


volatile static int8_t  S3 = 0;
volatile static uint16_t straghtflag3[4]={1,1,1,1};
volatile static uint16_t straghtflag2[4]={1,1,1,1};
volatile static uint16_t i=0;
/* 模式3*/
void Mode3_Switch(void)
{
    static uint32_t cnt = 0;
	static uint16_t count1=0;
	if(count1<=3)
	{ if(S3 == 0)
    {	
		if(straghtflag3[i]==1)
		{turn_in_place_start(90);
		targetturn=0;
			straghtflag3[i]=0;
		}
		targetspeed=3000;
        if(R1_raw||R2_raw||L1_raw||L2_raw)
        {
			cnt++;
            if(cnt > Black_CNT)
            {
              beep_start(200);
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
                beep_start(200);
                S3 = 2;
                cnt = 0;
            }
        } else cnt = 0;
    }
    else if(S3 == 2)
    {		
       targetspeed=0;
		if (fabsf(actualangle) < 0.5f)
		{
		S3=3;
		}        
    }
	else if(S3 == 3)
    {	
		
		if(straghtflag2[i]==1)
		{turn_in_place_start(90);	
     
		targetturn=0;
			straghtflag2[i]=0;
		}
		targetspeed=4000;
		if(R1_raw||R2_raw||L1_raw||L2_raw)
        {
			cnt++;
            if(cnt > Black_CNT)
            {
              beep_start(200);
                S3 = 4;
                cnt = 0;
            }
        } else cnt = 0;
        
	}
	 else if(S3 == 4)
    {	
        if(!(R1_raw||R2_raw||L1_raw||L2_raw))
        {	
			cnt++;
            if(cnt > White_CNT)
            {
                beep_start(200);
                S3 = 5;
                cnt = 0;
            }
        } else cnt = 0;
    }
    else if(S3 == 5)
    {		
     targetspeed=0;
		if (fabsf(actualangle) < 0.5f)
		{
		count1++;			
			S3=0;
			i++;
		}        
    }
	
}
if (count1 >= 4)
{
    for (int k = 0; k < 4; k++)
    {
        straghtflag2[k] = 1;
        straghtflag3[k] = 1;
    }
    count1 = 0;
    i = 0;
}	
}


