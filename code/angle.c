#include "mm32_device.h"                // Device header
#include "pid.h"// Device header
piddef positionpid;
piddef speedpid;
piddef anglepid;
void pid_init(void)
{
speedpid.jifen=0;
speedpid.jifenxianzhi=100;
	speedpid.kp=3.1f;
	speedpid.ki=0.3f;
	speedpid.kd=0.0f;
	speedpid.lasterror=0;
	speedpid.outxianzhi=99;
	positionpid.jifen=0;
positionpid.jifenxianzhi=100;
	positionpid.kp=1.2f;
positionpid.ki=0.01f;
positionpid.kd=0.0f;
	positionpid.lasterror=0;
positionpid.outxianzhi=999;
	    anglepid.jifen = 0;
    anglepid.jifenxianzhi = 300;
    anglepid.kp = 0;     
    anglepid.ki = 0;
    anglepid.kd = 0;
    anglepid.lasterror = 0;
    anglepid.outxianzhi = 1000;
}

int16_t pidcalculate(piddef*pid,float target,float actual)
{
float error=target-actual;
	float out;
	pid->jifen+=error;
	if(pid->jifen>pid->jifenxianzhi)
	{pid->jifen=pid->jifenxianzhi;}
	if(pid->jifen<-pid->jifenxianzhi)
	{pid->jifen=-pid->jifenxianzhi;}
	float dao=error-pid->lasterror;
	out=pid->kp*error+pid->ki*pid->jifen+pid->kd*dao;
	if(out>pid->outxianzhi)
	{out=pid->outxianzhi;}
	if(out<-pid->outxianzhi)
	{out=-pid->outxianzhi;}
	pid->lasterror=error;
	return out;
}
int16_t pidspeedcal(float targetspeed,float actualspeed)
{
return pidcalculate(&speedpid,targetspeed,actualspeed);
}
int16_t pidposcal(float targetpos,float actualpos)
{
return pidcalculate(&positionpid,targetpos,actualpos);
}
int16_t pidangcal(float targetang,float actualang)
{
return pidcalculate(&anglepid,targetang,actualang);
}




