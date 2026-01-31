#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_driver_pit.h"
#include "menu.h"
#include "key.h"
#include "zf_device_key.h"
#include "pid.h"
#include "flash.h"
#include "motor.h"
#include <stdint.h>
#include "encoder.h"
#include "zf_device_mpu6050.h"
#include "angle.h"
#include "TCRT.h"
#include "bluetooth.h"
#include "zf_driver_uart.h"
#include "vofa_firewater.h"
#include "isr.h"
#include "bluetooth.h"

int key;

void pit_tim1_init(void)
{
    pit_ms_init(TIM1_PIT, 1);   
}

int main(void)
	{
	clock_init(SYSTEM_CLOCK_120M);
		TCRT_Init();	
	gpio_init(C15, GPO, 1, GPO_PUSH_PULL);
	mpu6050_init();
	Angle_Init();
	pit_tim1_init();
    
    
	pid_init();
	key_init(10);     
    menu_init();
    motor_init();
		encoder_init();
		path_load_from_flash();
		bluetooth_init();
		uart_init(UART_1, 115200, UART1_TX_A9, UART1_RX_A10);
//		flash_param_load();  // 覆盖默认值（如果有）
		firewater_redirect_printf(" fuck \r\n");
    while(1)
    {  
		if(flagmenu==1)
		{     
		menu_display();  
		flagmenu=0;
		}
		
		key=key_get_event();		   // ★ 底层扫描
            menu_key_task(key); 
		if(rx_frame_ready) 
		{
				bluetooth_cmd_parse(rx_buf);
			
				// 处理完后一定要手动清除标志位，否则会反复进入
				rx_frame_ready = 0; 
		}
  switch(carmode)
    {
        case 1:
          targetspeed=0;
		targetturn=0;
            break;

        case 2:
		
      mode2flag=1;
            break;

        case 3:

		    mode3flag=1;
		
            break;

		case 4:
			 targetspeed=0;
			if(key_get_event()==KEY_LONG_DOWN)
			{  

			path_record_start();}
			if(key_get_event()==KEY_LONG_BACK)
			{  
			
			path_record_stop_and_save();}
			
			if(key_get_event()==KEY_LONG_up)
			{
			 path_load_from_flash();
			replay_start();
			}
				//放
				break;

        case 5:
         if(rx_frame_ready) 
		{
				bluetooth_cmd_parse(rx_buf);
			
				// 处理完后一定要手动清除标志位，否则会反复进入
				rx_frame_ready = 0; 
		}
            break;
    }	
firewater_send_float(CarAngle);
//		firewater_send_int(angleout);
	//	uart_printf(UART_6,"[plot,%f]",CarAngle);

    }
}