#ifndef __TCRT_H_
#define __TCRT_H_

#include "zf_common_typedef.h"
#include "BEEP.h"

//#define	R1_raw   gpio_get_level (A0)
//#define	R2_raw   gpio_get_level (A1) 
//#define	L1_raw   gpio_get_level (A2)
//#define	L2_raw   gpio_get_level (A3)	//原始数据直接读取

void TCRT_Init(void);
void TCRT_Update(void);
void TCRT_Trace(void);
void Mode2_Switch(void);
void Mode3_Switch(void);

extern uint16_t mode2flag;
#endif
