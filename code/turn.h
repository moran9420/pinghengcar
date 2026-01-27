#ifndef _TURN_CTRL_H_
#define _TURN_CTRL_H_

#include <stdint.h>

// 启动原地转向（单位：度，正=右转，负=左转）
void turn_in_place_start(float angle_deg);

// 每个控制周期调用（放在 TIM 中断里）
void turn_in_place_step(void);

// 是否正在转向
extern uint8_t turn_active;

#endif
