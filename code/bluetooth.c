#include "bluetooth.h"
#include "zf_driver_uart.h"
#include "pid.h"
#include "motor.h"
/* ---------------- 状态机枚举 ---------------- */
typedef enum
{
    RX_WAIT_HEAD = 0,
    RX_COLLECT_DATA
} rx_state_t;

/* ---------------- 状态机变量 ---------------- */
static rx_state_t rx_state = RX_WAIT_HEAD;
static uint8_t rx_cnt = 0;

char rx_buf[RX_BUF_LEN];              // 接收缓冲
volatile uint8_t rx_frame_ready = 0;  // 帧完成标志

void bluetooth_init()
{
	uart_init(UART_6, 9600, UART6_TX_C6, UART6_RX_C7);
	uart_rx_interrupt(UART_6,1);
}
void bluetooth_rx_sm(uint8_t ch)
{
    switch (rx_state)
    {
        case RX_WAIT_HEAD:
            if (ch == '[')
            {
                rx_cnt = 0;
                rx_state = RX_COLLECT_DATA;
            }
            break;

        case RX_COLLECT_DATA:
            if (ch == ']')
            {
                rx_buf[rx_cnt] = '\0';  // 字符串结束
                rx_state = RX_WAIT_HEAD;
                rx_frame_ready = 1;     // 标记帧完成
            }
            else if (rx_cnt < RX_BUF_LEN - 1)
            {
                rx_buf[rx_cnt++] = ch;  // 收集数据
            }
            else
            {
                // 超出缓冲，丢弃本帧
                rx_cnt = 0;
                rx_state = RX_WAIT_HEAD;
            }
            break;
    }
}
void bluetooth_cmd_parse(char *cmd)
{
    char type[16];
    char name[16];
    float value;

    // --- 1. 解析参数类指令 (格式: type,name,value) ---
    // 例如: "slider,kp,1.5" 或 "slider,ki,0.05"
    if (sscanf(cmd, "%15[^,],%15[^,],%f", type, name, &value) == 3)
    {
        if (strcmp(type, "slider") == 0)
        {
            if (strcmp(name, "angle.kp") == 0)
            {
                anglepid.kp = value;
                 // 保持您原有的反馈
            }
            else if (strcmp(name, "angle.ki") == 0)
            {
                anglepid.ki = value;
                // 这里可以添加发送 kp 值的反馈，例如:
           
            }
            else if (strcmp(name, "angle.kd") == 0)
            {
                anglepid.kd = value;
              
            }
			 else if (strcmp(name, "speed.kp") == 0)
            {
                speedpid.kp = value;
                // 这里可以添加发送 kp 值的反馈，例如:
           
            }
            else if (strcmp(name, "speed.ki") == 0)
            {
                speedpid.ki = value;
              
            }
			 else if (strcmp(name, "speed.kd") == 0)
            {
                speedpid.kd = value;
                // 这里可以添加发送 kp 值的反馈，例如:
           
            }
            else if (strcmp(name, "turn.kd") == 0)
            {
                turnpid.kd = value;
              
            }
			 else if (strcmp(name, "turn.kp") == 0)
            {
                turnpid.kp = value;
                // 这里可以添加发送 kp 值的反馈，例如:
           
            }
            else if (strcmp(name, "turn.ki") == 0)
            {
                turnpid.ki= value;
              
            }
        }
        return; // 解析成功参数后直接返回
    }

    // --- 2. 解析状态机控制指令 (纯字符串) ---
    
    if (strcmp(cmd, "go ahead") == 0)
    {
       Car_Start();
    }
    else if (strcmp(cmd, "turn left") == 0)
    {
       targetturn=2000;
    }
    else if (strcmp(cmd, "turn right") == 0)
    {
        targetturn=-2000;
    }
    else if (strcmp(cmd, "stop") == 0)
    {
		Car_Stop();      
    }

    else if (strcmp(cmd, "go back") == 0)
    {
		Car_back();
    }
}