/*********************************************************************************************************************
* 文件名：vofa_firewater.c
* 功能：VOFA+ FireWater协议 printf 重定向实现
********************************************************************************************************************/

#include "vofa_firewater.h"

// 全局变量
static uart_index_enum g_default_uart = UART_1;
static uint32_t g_frame_count = 0;
static uint32_t g_byte_count = 0;
static char g_firewater_buffer[512];  // FireWater协议缓冲区

// 初始化
void firewater_init(uart_index_enum default_uart, uint32_t baudrate)
{
    g_default_uart = default_uart;
    g_frame_count = 0;
    g_byte_count = 0;
    
    // 注意：UART初始化需要在外部调用，这里只设置默认串口
    // uart_init(g_default_uart, baudrate, ...);
}

// 发送单个浮点数
void firewater_send_float(float data)
{
    char buffer[32];
    int len = snprintf(buffer, sizeof(buffer), "%.6f%c", data, FIRewater_TERMINATOR);
    
    for(int i = 0; i < len; i++) {
        uart_write_byte(g_default_uart, buffer[i]);
    }
    
    g_frame_count++;
    g_byte_count += len;
}

// 发送单个整数
void firewater_send_int(int data)
{
    char buffer[32];
    int len = snprintf(buffer, sizeof(buffer), "%d%c", data, FIRewater_TERMINATOR);
    
    for(int i = 0; i < len; i++) {
        uart_write_byte(g_default_uart, buffer[i]);
    }
    
    g_frame_count++;
    g_byte_count += len;
}

// 发送字符串
void firewater_send_string(const char* str)
{
    if(str == NULL) return;
    
    // 发送字符串
    while(*str) {
        uart_write_byte(g_default_uart, *str++);
        g_byte_count++;
    }
    
    // 发送结束符
    uart_write_byte(g_default_uart, FIRewater_TERMINATOR);
    g_byte_count++;
    g_frame_count++;
}

// 发送多个浮点数（可变参数）
void firewater_send_floats(int count, ...)
{
    if(count <= 0) return;
    
    va_list args;
    va_start(args, count);
    
    // 构建FireWater格式字符串
    int pos = 0;
    for(int i = 0; i < count; i++) {
        float value = (float)va_arg(args, double);  // float在可变参数中提升为double
        int len = snprintf(&g_firewater_buffer[pos], 
                          sizeof(g_firewater_buffer) - pos,
                          "%.6f", value);
        
        if(i < count - 1) {
            g_firewater_buffer[pos + len] = ',';
            pos += len + 1;
        } else {
            pos += len;
        }
    }
    va_end(args);
    
    // 添加结束符
    g_firewater_buffer[pos++] = FIRewater_TERMINATOR;
    g_firewater_buffer[pos] = '\0';
    
    // 发送数据
    for(int i = 0; i < pos; i++) {
        uart_write_byte(g_default_uart, g_firewater_buffer[i]);
    }
    
    g_frame_count++;
    g_byte_count += pos;
}

// 发送多个整数（可变参数）
void firewater_send_ints(int count, ...)
{
    if(count <= 0) return;
    
    va_list args;
    va_start(args, count);
    
    // 构建FireWater格式字符串
    int pos = 0;
    for(int i = 0; i < count; i++) {
        int value = va_arg(args, int);
        int len = snprintf(&g_firewater_buffer[pos], 
                          sizeof(g_firewater_buffer) - pos,
                          "%d", value);
        
        if(i < count - 1) {
            g_firewater_buffer[pos + len] = ',';
            pos += len + 1;
        } else {
            pos += len;
        }
    }
    va_end(args);
    
    // 添加结束符
    g_firewater_buffer[pos++] = FIRewater_TERMINATOR;
    g_firewater_buffer[pos] = '\0';
    
    // 发送数据
    for(int i = 0; i < pos; i++) {
        uart_write_byte(g_default_uart, g_firewater_buffer[i]);
    }
    
    g_frame_count++;
    g_byte_count += pos;
}

// FireWater格式的printf（末尾自动添加换行）
int firewater_printf(const char* format, ...)
{
    if(format == NULL) return -1;
    
    va_list args;
    int total_len;
    
    // 格式化字符串
    va_start(args, format);
    total_len = vsnprintf(g_firewater_buffer, sizeof(g_firewater_buffer), format, args);
    va_end(args);
    
    // 检查缓冲区是否足够
    if(total_len < 0) {
        return -1;  // 格式化错误
    }
    
    if(total_len >= (int)sizeof(g_firewater_buffer)) {
        // 缓冲区不足，截断
        total_len = sizeof(g_firewater_buffer) - 1;
    }
    
    // 确保以换行符结束（如果还没有）
    if(total_len == 0 || 
       (g_firewater_buffer[total_len-1] != '\n' && 
        g_firewater_buffer[total_len-1] != '\r')) {
        
        if(total_len < (int)sizeof(g_firewater_buffer) - 1) {
            g_firewater_buffer[total_len++] = FIRewater_TERMINATOR;
            g_firewater_buffer[total_len] = '\0';
        }
    }
    
    // 发送数据
    for(int i = 0; i < total_len; i++) {
        uart_write_byte(g_default_uart, g_firewater_buffer[i]);
    }
    
    g_frame_count++;
    g_byte_count += total_len;
    
    return total_len;
}

// 原始数据发送
void firewater_send_raw(const char* data, uint32_t length)
{
    if(data == NULL || length == 0) return;
    
    for(uint32_t i = 0; i < length; i++) {
        uart_write_byte(g_default_uart, data[i]);
    }
    
    g_byte_count += length;
}

// 重定向标准printf到FireWater协议
int firewater_redirect_printf(const char* format, ...)
{
    if(format == NULL) return -1;
    
    va_list args;
    int total_len;
    
    // 格式化字符串
    va_start(args, format);
    total_len = vsnprintf(g_firewater_buffer, sizeof(g_firewater_buffer), format, args);
    va_end(args);
    
    if(total_len <= 0) return total_len;
    
    if(total_len >= (int)sizeof(g_firewater_buffer)) {
        total_len = sizeof(g_firewater_buffer) - 1;
    }
    
    // 发送数据
    for(int i = 0; i < total_len; i++) {
        uart_write_byte(g_default_uart, g_firewater_buffer[i]);
    }
    
    g_byte_count += total_len;
    
    return total_len;
}

// 获取统计信息
uint32_t firewater_get_frame_count(void)
{
    return g_frame_count;
}

uint32_t firewater_get_byte_count(void)
{
    return g_byte_count;
}
