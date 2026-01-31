#ifndef _ANGLE_H_
#define _ANGLE_H_

#include "zf_common_typedef.h"

// 外部可以引用的姿态角
extern float CarAngle; // 平衡控制通常使用 Pitch 或 Roll，取决于安装方向
extern float Roll, Pitch, Yaw; 

void Angle_Init(void);
void Angle_Update(void);   // 建议在 5ms 或 10ms 中断中调用

#endif