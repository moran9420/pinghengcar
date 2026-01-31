#include "pid.h"
#include "menu.h"
#include "key.h"
#include "zf_device_oled.h"
#include <stdio.h>
#include "flash.h"



/* ================= 菜单状态变量 ================= */
unsigned char currentmenu = 0;   // 0主菜单 1模式 2PID环 3PID参数
unsigned char biao = 0;          // 光标
unsigned char pid_item = 0;      // 0=KP 1=KI 2=KD
unsigned char pid_loop = 0;      // 0角度 1速度 2位置

unsigned char carmode = 1;
unsigned char flagmenu = 1;      // 上电先刷一次
static uint8_t long_ok_handled = 0;
/* ================= 初始化 ================= */
void menu_init(void)
{
    oled_init();
    oled_clear();
    flagmenu = 1;
}

/* ================= 按键处理 ================= */
void menu_key_task(int key)
{
    
   if(key == KEY_NONE) 
    {
        long_ok_handled = 0;  // 在无按键时清除长按标志
        return;
    }

   

if(key == KEY_LONG_OK && !long_ok_handled)
    {
        long_ok_handled = 1;
        
        // 只在PID参数调整界面保存
        if(currentmenu == 3)
        {
            flash_param_save();  // 保存PID参数
            
            // 显示保存提示
            oled_clear();
            oled_show_string(20, 2, "SAVED!");
            flagmenu = 1;
        }
        return;  // 长按处理完毕，直接返回
    }
	/* ---------- 主菜单 ---------- */
    if(currentmenu == 0)
    {
        if(key == KEY_UP)
        {
            if(biao == 0) biao = 1;
            else biao--;
            flagmenu = 1;
        }
        else if(key == KEY_DOWN)
        {
            biao = (biao + 1) % 2;
            flagmenu = 1;
        }
        else if(key == KEY_OK)
        {
            if(biao == 0) currentmenu = 1;   // MODE
            if(biao == 1) currentmenu = 2;   // PID
            biao = 0;
            flagmenu = 1;
        }
    }

    else if(currentmenu == 1)
    {
        if(key == KEY_BACK)
        {	carmode=1;
            currentmenu = 0;
            biao = 0;
            flagmenu = 1;
			
        }
        else if(key == KEY_UP)
        {
            if(biao == 0) biao = 4;
            else biao--;
            flagmenu = 1;
        }
        else if(key == KEY_DOWN)
        {
            biao = (biao + 1) % 5;
            flagmenu = 1;
        }
        else if(key == KEY_OK)
        {
            carmode = biao + 1;   // 1~5
            flagmenu = 1;
        }
    }

    /* ---------- PID 环选择 ---------- */
    else if(currentmenu == 2)
    {
        if(key == KEY_BACK)
        {
            currentmenu = 0;
            biao = 0;
            flagmenu = 1;
        }
        else if(key == KEY_UP)
        {
            if(biao == 0) biao = 2;
            else biao--;
            flagmenu = 1;
        }
        else if(key == KEY_DOWN)
        {
            biao = (biao + 1) % 3;
            flagmenu = 1;
        }
        else if(key == KEY_OK)
        {
            pid_loop = biao;   // 0角度 1速度 2位置
            pid_item = 0;
            currentmenu = 3;
            flagmenu = 1;
        }
    }

    /* ---------- PID 参数调整 ---------- */
    
	
	
	
	
	else if(currentmenu == 3)
    {
        piddef *pid;

        if(pid_loop == 0) pid = &anglepid;
        else if(pid_loop == 1) pid = &speedpid;
        else pid = &turnpid;
		
  

    /* ----- 长按一次性处理 ----- */
  

        if(key == KEY_BACK)
        {
            currentmenu = 2;
            biao = pid_loop;
            flagmenu = 1;
        }
        else if(key == KEY_OK)
        {
            pid_item = (pid_item + 1) % 3;
            flagmenu = 1;
        }
        else if(key == KEY_UP)
        {
            if(pid_item == 0) pid->kp += 0.1f;
            if(pid_item == 1) pid->ki += 0.01f;
            if(pid_item == 2) pid->kd += 0.1f;
            flagmenu = 1;
        }
        else if(key == KEY_DOWN)
        {
            if(pid_item == 0) pid->kp -= 0.1f;
            if(pid_item == 1) pid->ki -= 0.01f;
            if(pid_item == 2) pid->kd -= 0.1f;
            flagmenu = 1;
        }
    }
}

/* ================= OLED 显示 ================= */
void menu_display(void)
{
    char buf[16];
    piddef *pid;

    if(flagmenu == 0) return;
    flagmenu = 0;

    oled_clear();

    if(currentmenu == 0)
    {
        oled_show_string(10,1,"MODE");
        oled_show_string(10,2,"PID");
        oled_show_string(1,biao+1,">");
    }
    else if(currentmenu == 1)
    {
        for(int i=0;i<5;i++)
        {
            sprintf(buf,"MODE %d",i+1);
            oled_show_string(10,i+1,buf);
        }
        oled_show_string(1,biao+1,">");
    }
    else if(currentmenu == 2)
    {
        oled_show_string(10,1,"ANGLE");
        oled_show_string(10,2,"SPEED");
        oled_show_string(10,3,"POSITION");
        oled_show_string(1,biao+1,">");
    }
    else if(currentmenu == 3)
    {
        if(pid_loop == 0) pid = &anglepid;
        else if(pid_loop == 1) pid = &speedpid;
        else pid = &turnpid;

        sprintf(buf,"KP %.2f",pid->kp);
        oled_show_string(10,1,buf);
        sprintf(buf,"KI %.2f",pid->ki);
        oled_show_string(10,2,buf);
        sprintf(buf,"KD %.2f",pid->kd);
        oled_show_string(10,3,buf);

        oled_show_string(1,pid_item+1,">");
    }
}
