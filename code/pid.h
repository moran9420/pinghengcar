#ifndef __PID_H
#define __PID_H
#include <stdint.h>

typedef struct
{
float kp;
float ki;
float kd;
float jifen;
float lasterror;
float jifenxianzhi;
float outxianzhi;
}piddef;
extern piddef speedpid;
extern piddef turnpid;
extern piddef anglepid;
void pid_init(void);
int16_t pidangcal(float targetang, float actualang, float gyro_speed);
int16_t pid_angle_calculate(piddef* pid, float target, float actual, float gyro_speed);
int16_t pidspeedcal(float targetspeed,float actualspeed);
int16_t pidturncal(float targetpos,float actualpos);

#endif
