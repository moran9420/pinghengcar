           // Device header
#include "zf_driver_flash.h"
#include "flash.h"
#include "motor.h"
#include "menu.h"
flash_param_t flash_param;

path_sample_t path_buffer[4096]; // 路径 RAM 缓冲区
uint32_t path_length = 0;        // 已记录路径点数
uint8_t path_record_enable = 0; 
uint8_t replay_enable = 0;   // 正在重放// 路径记录使能标志
void flash_param_save(void)
{
    flash_param.magic   = FLASH_MAGIC;


    flash_param.angle    = anglepid;
    flash_param.speed    = speedpid;
    flash_param.turn     = turnpid;

    flash_write_page(
        FLASH_SECTOR,
        FLASH_PAGE,
        (uint32 *)&flash_param,
        sizeof(flash_param_t) / 4
    );
}
void flash_param_load(void)
{
    flash_read_page(
        FLASH_SECTOR,
        FLASH_PAGE,
        (uint32 *)&flash_param,
        sizeof(flash_param_t) / 4
    );

    if(flash_param.magic != FLASH_MAGIC)
    {
        
        return;
    }

    anglepid    = flash_param.angle;
    speedpid    = flash_param.speed;
    turnpid = flash_param.turn;
}
void path_record_start(void)
{
    path_length = 0;                          // 清空路径长度
    path_record_enable = 1;                  // 允许记录
}


void path_record_push(int16_t speed, int16_t turn)
{
    if(path_record_enable == 0) return;       // 未使能直接退出
    if(path_length >= 4096) return;           // 防止越界

    path_buffer[path_length].speed = speed;  // 保存速度
    path_buffer[path_length].turn  = turn;   // 保存转向

    path_length++;                            // 路径点 +1
}

/* =========================
   停止记录并保存到 Flash
   ========================= */
void path_record_stop_and_save(void)
{
    uint32_t page_buffer[256];                // 单页缓冲区
    uint32_t write_index = 0;                 // 已写入路径点数

    path_record_enable = 0;                   // 关闭记录

    for(uint32_t page = 0; page < 16; page++)
    {
        for(uint32_t i = 0; i < 256; i++)
        {
            if(write_index < path_length)
            {
                page_buffer[i] =
                    ((uint16_t)path_buffer[write_index].turn << 16) |
                    ((uint16_t)path_buffer[write_index].speed);
                write_index++;
            }
            else
            {
                page_buffer[i] = 0xFFFFFFFF;  // 剩余填空
            }
        }

        flash_write_page(
            120 + page / 4,                   // 扇区号
            page % 4,                         // 页号
            page_buffer,                      // 写入数据
            256                               // word 数
        );
    }
}

/* =========================
   从 Flash 读取路径
   ========================= */
uint8_t path_load_from_flash(void)
{
    uint32_t page_buffer[256];                // 页缓冲
    uint32_t read_index = 0;                  // 读取索引

    path_length = 0;                          // 清空路径

    for(uint32_t page = 0; page < 16; page++)
    {
        flash_read_page(
            120 + page / 4,                   // 扇区号
            page % 4,                         // 页号
            page_buffer,                      // 读取到缓冲区
            256                               // word 数
        );

        for(uint32_t i = 0; i < 256; i++)
        {
            if(page_buffer[i] == 0xFFFFFFFF)  // 遇到空数据停止
            {
                return 1;                    // 读取完成
            }

            path_buffer[read_index].speed =
                (int16_t)(page_buffer[i] & 0xFFFF);

            path_buffer[read_index].turn =
                (int16_t)(page_buffer[i] >> 16);

            read_index++;
            path_length++;
        }
    }

    return 1;                                // 成功
}
static uint32_t replay_index = 0;   // 当前重放到第几个点
         // 重放使能标志

void replay_start(void)
{
    replay_index = 0;               // 从第 0 个点开始
    replay_enable = 1;              // 打开重放
}
void replay_step(void)
{
    // 如果没开重放，直接退出
    if(replay_enable == 0) return;

    // 如果已经放完了
    if(replay_index >= path_length)
    {
        replay_enable = 0;   
        targetturn = 0; 
		carmode=1;		
        return;
    }

    // ⭐ 核心：把记录的 turn 原样送给 PID
    targetturn = path_buffer[replay_index].turn;
	targetspeed = path_buffer[replay_index].speed;

    replay_index++;          // 下一个点
}