#ifndef _ENCODER_H_
#define _ENCODER_H_
#include "zf_common_typedef.h"

// 初始化两个编码器
void encoder_init(void);

// 左编码器
int16 encoder_left_speed(void);     // 速度（清零）
int32 encoder_left_position(void);  // 位置（不清零）
void  encoder_left_reset(void);

// 右编码器
int16 encoder_right_speed(void);
int32 encoder_right_position(void);
void  encoder_right_reset(void);

#endif

