/*********************************************************************************************************************
* 文件名：vofa_firewater.h
* 功能：VOFA+ FireWater协议 printf 重定向
* 协议：文本格式，逗号分隔，换行结束
********************************************************************************************************************/

#ifndef _VOFA_FIREWATER_H_
#define _VOFA_FIREWATER_H_

#include "zf_driver_uart.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// FireWater 协议控制字符
#define FIRewater_TERMINATOR    '\n'    // 协议结束符，也可以使用 "\r\n"

// 初始化 FireWater 输出（设置默认串口和参数）
void firewater_init(uart_index_enum default_uart, uint32_t baudrate);

// 发送单个数据（自动添加逗号和换行）
void firewater_send_float(float data);
void firewater_send_int(int data);
void firewater_send_string(const char* str);

// 发送多个数据（逗号分隔，自动换行）
void firewater_send_floats(int count, ...);    // 可变参数，发送多个浮点数
void firewater_send_ints(int count, ...);      // 可变参数，发送多个整数

// FireWater 格式的 printf（末尾自动添加换行）
int firewater_printf(const char* format, ...);

// 原始数据发送（不添加任何格式，用于自定义格式）
void firewater_send_raw(const char* data, uint32_t length);

// 标准 printf 重定向到 FireWater 协议
int firewater_redirect_printf(const char* format, ...);

// 获取发送统计
uint32_t firewater_get_frame_count(void);
uint32_t firewater_get_byte_count(void);

#endif /* _VOFA_FIREWATER_H_ */
