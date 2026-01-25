#ifndef __FLASH_PARAM_H
#define __FLASH_PARAM_H

#include "pid.h"
#include <stdint.h>

#define FLASH_SECTOR   127     // 最后一个扇区（安全）
#define FLASH_PAGE     3       // 扇区的最后一页
#define FLASH_MAGIC    0x5AA55AA5

typedef struct
{
    uint32_t magic;       // 校验
    uint8_t  carmode;

    piddef angle;
    piddef speed;
    piddef position;

} flash_param_t;

void flash_param_save(void);
void flash_param_load(void);

#endif
