#ifndef __FLASH_PARAM_H
#define __FLASH_PARAM_H

#include "pid.h"
#include <stdint.h>


#define FLASH_SECTOR   127     // 最后一个扇区（安全）
#define FLASH_PAGE     3       // 扇区的最后一页
#define FLASH_MAGIC    0x5AA55AA5
#define FLASH_PATH_PAGE 2  
typedef struct
{
    uint32_t magic;       // 校验
    uint8_t  carmode;

    piddef angle;
    piddef speed;
    piddef turn;

} flash_param_t;
typedef struct
{
    int16_t speed;           // 当时小车速度
    int16_t turn;            // 当时小车转向
} path_sample_t;

/* =========================
   对外变量声明
   ========================= */
extern flash_param_t flash_param;        // PID 参数 Flash 缓存
extern path_sample_t path_buffer[];      // 路径 RAM 缓冲区
extern uint32_t path_length;              // 当前已记录的路径点数量
extern uint8_t path_record_enable;        // 路径记录使能标志

/* =========================
   函数声明
   ========================= */
void flash_param_save(void);              // 保存 PID 到 Flash
void flash_param_load(void);              // 从 Flash 读取 PID

void path_record_start(void);             // 开始路径记录（清 RAM）
void path_record_stop_and_save(void);     // 停止记录并写入 Flash
void path_record_push(int16_t speed, int16_t turn); // 记录一个路径点（中断调用）
void replay_step(void);
void replay_start(void);
uint8_t path_load_from_flash(void); 
extern uint8_t replay_enable ;// 从 Flash 读取路径   // 加载路径数据
#endif
