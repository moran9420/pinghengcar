#include "mm32_device.h"                // Device header
#include "zf_driver_flash.h"
#include "flash.h"
flash_param_t flash_param;
void flash_param_save(void)
{
    flash_param.magic   = FLASH_MAGIC;


    flash_param.angle    = anglepid;
    flash_param.speed    = speedpid;
    flash_param.position = positionpid;

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
        // Flash 没存过数据，直接用默认 PID
        return;
    }

    anglepid    = flash_param.angle;
    speedpid    = flash_param.speed;
    positionpid = flash_param.position;
}