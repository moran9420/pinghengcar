#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include "zf_common_typedef.h"

#define RX_BUF_LEN 128

extern char rx_buf[RX_BUF_LEN];             
extern volatile uint8_t rx_frame_ready;     

void bluetooth_init();
void bluetooth_rx_sm(uint8_t ch);
void bluetooth_cmd_parse(char *cmd);

#endif