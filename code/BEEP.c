#include <stdint.h>
#include "zf_driver_gpio.h"
#include "zf_driver_delay.h"

#include "BEEP.h"
uint16_t beep_cnt;
void Beep_Init(void)
{
	gpio_init (BEEP_PIN, GPO, 1, GPO_PUSH_PULL);//0 响，1 不响
}

void Beep_Once(void)  //响一次
{
    BEEP_ON();
	system_delay_ms(50);
    BEEP_OFF();
}
void beep_start(uint16_t time_ms)
{
    BEEP_ON();            // GPIO 拉高
    beep_cnt = time_ms;   // 设置响的时间
}