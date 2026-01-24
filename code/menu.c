
#include "menu.h"
#include "key.h"
#include "zf_device_oled.h"
#include <stdio.h>

/* 页面编号（直接 #define） */
#define PAGE_MAIN   0
#define PAGE_MODE   1
#define PAGE_PID    2   // 只是占位，不放参数

int menu_mode   = 1;
int menu_page   = PAGE_MAIN;
int menu_cursor = 0;

void menu_init(void)
{
    oled_init();
    oled_clear();
}

void menu_task(void)
{
    int key = key_get_event();

    /* ===== 按键处理 ===== */
    if(key == KEY_UP)
    {
        if(menu_cursor > 0) menu_cursor--;
    }
    else if(key == KEY_DOWN)
    {
        menu_cursor++;
    }
    else if(key == KEY_OK)
    {
        if(menu_page == PAGE_MAIN)
        {
            if(menu_cursor == 0) menu_page = PAGE_MODE;
            if(menu_cursor == 1) menu_page = PAGE_PID;
            menu_cursor = 0;
        }
        else if(menu_page == PAGE_MODE)
        {
            menu_mode = menu_cursor + 1;   // 1~5
        }
    }
    else if(key == KEY_BACK)
    {
        menu_page = PAGE_MAIN;
        menu_cursor = 0;
    }

    /* ===== OLED 显示 ===== */
    oled_clear();

    if(menu_page == PAGE_MAIN)
    {
        oled_show_string(0,0,"> MAIN");
        oled_show_string(0,2,menu_cursor==0?"> MODE":"  MODE");
        oled_show_string(0,3,menu_cursor==1?"> PID ":"  PID ");
    }
    else if(menu_page == PAGE_MODE)
    {
        oled_show_string(0,0,"> MODE");
        for(int i=0;i<5;i++)
        {
            char buf[12];
            sprintf(buf,"%c MODE %d",(menu_cursor==i)?'>':' ',i+1);
            oled_show_string(0,i+2,buf);
        }
    }
    else if(menu_page == PAGE_PID)
    {
        oled_show_string(0,0,"> PID PAGE");
        oled_show_string(0,3,"(no param here)");
    }
}
