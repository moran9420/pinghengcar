#include "mm32_device.h"                // Device header
#include "encoder.h"
#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_driver_timer.h"
#include "zf_driver_encoder.h"

// ================== 硬件映射 ==================

// 左编码器
#define LEFT_ENCODER_TIMER   TIM3_ENCODER
#define LEFT_ENCODER_CH1     TIM3_ENCODER_CH1_B4
#define LEFT_ENCODER_CH2     TIM3_ENCODER_CH2_B5

// 右编码器
#define RIGHT_ENCODER_TIMER  TIM4_ENCODER
#define RIGHT_ENCODER_CH1    TIM4_ENCODER_CH1_B6
#define RIGHT_ENCODER_CH2    TIM4_ENCODER_CH2_B7

// ==============================================

static int32 left_position  = 0;
static int32 right_position = 0;

//-------------------------------------------------------------------------------------------------------------------
// 初始化两个编码器
//-------------------------------------------------------------------------------------------------------------------
void encoder_init(void)
{
    // 左编码器
    encoder_quad_init(
        LEFT_ENCODER_TIMER,
        LEFT_ENCODER_CH1,
        LEFT_ENCODER_CH2
    );
    encoder_clear_count(LEFT_ENCODER_TIMER);

    // 右编码器
    encoder_quad_init(
        RIGHT_ENCODER_TIMER,
        RIGHT_ENCODER_CH1,
        RIGHT_ENCODER_CH2
    );
    encoder_clear_count(RIGHT_ENCODER_TIMER);

    left_position  = 0;
    right_position = 0;
}


int16 encoder_left_speed(void)
{
    int16 delta = encoder_get_count(LEFT_ENCODER_TIMER);
    encoder_clear_count(LEFT_ENCODER_TIMER);
    left_position += delta;
    return delta;
}


int16 encoder_right_speed(void)
{
    int16 delta = encoder_get_count(RIGHT_ENCODER_TIMER);
    encoder_clear_count(RIGHT_ENCODER_TIMER);
    right_position += delta;
    return delta;
}


int32 encoder_left_position(void)
{
    return left_position;
}


int32 encoder_right_position(void)
{
    return right_position;
}


void encoder_left_reset(void)
{
    encoder_clear_count(LEFT_ENCODER_TIMER);
    left_position = 0;
}


void encoder_right_reset(void)
{
    encoder_clear_count(RIGHT_ENCODER_TIMER);
    right_position = 0;
}
