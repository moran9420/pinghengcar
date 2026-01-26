#ifndef __BEEP_H_
#define __BEEP_H_

#include "zf_common_typedef.h"

#define BEEP_PIN    C15
#define BEEP_ON()   gpio_set_level(BEEP_PIN, 0)
#define BEEP_OFF()  gpio_set_level(BEEP_PIN, 1)

void Beep_Init(void);
void Beep_Once(void);  //响一次
extern uint16_t beep_cnt;
void beep_start(uint16_t time_ms);
#endif
