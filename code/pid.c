        // Device header
#include "pid.h"// Device header
piddef turnpid;
piddef speedpid;
piddef anglepid;
	void pid_init(void)
	{
	speedpid.jifen=0;
	speedpid.jifenxianzhi=2;
		speedpid.kp=0.2f;
		speedpid.ki=0.00f;
		speedpid.kd=0.0f;
		speedpid.lasterror=0;
		speedpid.outxianzhi=15;
		turnpid.jifen=0;
	turnpid.jifenxianzhi=6000;
		turnpid.kp=1.2f;
	turnpid.ki=0.01f;
	turnpid.kd=0.0f;
		turnpid.lasterror=0;
	turnpid.outxianzhi=5000;
	anglepid.jifen = 0;
		anglepid.jifenxianzhi = 0;   // 直立环通常不需要积分，设为0或很小
		anglepid.kp = 12.0f;         // P项：力度不够车立不起来，太大车会剧烈高频震荡
		anglepid.ki = 0.0f;          // I项：通常保持为0
		anglepid.kd =-0.04f;          // D项：抑制震荡，太大会产生噪音，太小车会摇晃
		anglepid.lasterror = 0;
		anglepid.outxianzhi = 90;  // 输出限幅，根据电机PWM量程设定
	}

// pid.c 中修改这个包装函数
int16_t pidangcal(float targetang, float actualang, float gyro_speed)
{
    // 依然利用原有的 pidcalculate 计算 P 和 I
    // 但我们将微分项单独处理，因为角度差值产生的微分在 3ms 周期下太不准了
    float error = targetang - actualang;
    
    // 1. 计算 P 项和 I 项 (借用原函数逻辑)
    // 注意：这里暂时给内部 kd 传 0，因为我们要用外部真实的 gyro_speed 替换它
    float p_out = anglepid.kp * error;
    
    // 2. 积分逻辑保留
    anglepid.jifen += error;
    // ... 积分限幅逻辑 ...
    
    // 3. 核心：使用真实的陀螺仪角速度作为 D 项
    // gyro_speed 是物理值 (°/s)，对应 anglepid.kd
    float d_out = anglepid.kd * gyro_speed;
    
    float out = p_out + d_out; // 加上积分项(如有)
    
    // 4. 限幅并返回
    if(out > anglepid.outxianzhi) out = anglepid.outxianzhi;
    if(out < -anglepid.outxianzhi) out = -anglepid.outxianzhi;
    
    return (int16_t)out;
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
int16_t pidturncal(float targetturn,float actualturn)
{
return pidcalculate(&turnpid,targetturn,actualturn);
}





