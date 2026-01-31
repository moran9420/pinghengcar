#include "turn.h"
#include "encoder.h"
#include "pid.h"
#include "motor.h"
#include <math.h>
#include "isr.h"

/* ================== 物理参数 ================== */
#define WHEEL_DISTANCE   0.160f   // 左右轮间距（米，自己量）
#define DEG2RAD          0.017453292f

/* ================== 状态变量 ================== */
static int32_t left_start  = 0;
static int32_t right_start = 0;

static float target_diff_pos = 0.0f;   // 目标左右轮“位移差”
uint8_t turn_active = 0;

/* =================================================
 * 启动原地转向
 * ================================================= */
void turn_in_place_start(float angle_deg)
{
 
    left_start  = encoder_left_position();
    right_start = encoder_right_position();

    /* 
     * 目标几何关系：
     *  theta = (R - L) / W
     *  => (R - L) = theta * W
     */
    target_diff_pos = angle_deg *1200;

    /* 清 PID 积分，避免历史残留 */
    turnpid.jifen = 0;
    turnpid.lasterror = 0;

    turn_active = 1;
}

/* =================================================
 * 每个控制周期调用
 * ================================================= */
void turn_in_place_step(void)
{
    if (!turn_active) return;

    /* 当前左右轮位移（相对起点） */
    float left_pos  = (float)(encoder_left_position()  - left_start);
    float right_pos = (float)(encoder_right_position() - right_start);

    /* 当前实际位移差 */
    float diff_pos = right_pos - left_pos;

    /* PID 误差 */
    float error = target_diff_pos - diff_pos;

    /* === 终止条件 === */
    if (fabsf(error) < 2.0f)   // 编码器阈值，自己调
    {
        turn_active = 0;
        return;
    }

 
    motor_set_left (angleout - turnout);
    motor_set_right(angleout + turnout);
}


