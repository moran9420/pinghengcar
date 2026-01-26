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
int16_t pidcalculate(piddef*pid,float target,float actual);
int16_t pidspeedcal(float targetspeed,float actualspeed);
int16_t pidturncal(float targetpos,float actualpos);
int16_t pidangcal(float targetang,float actualang);
#endif
