#include "key.h"
#include "zf_device_key.h"

int key_get_event(void)
{
    if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
    {
        key_clear_state(KEY_1);
        return KEY_UP;
    }
	if(key_get_state(KEY_1) == KEY_LONG_PRESS)
    {
        key_clear_state(KEY_1);
        return KEY_LONG_up;
    }
    if(key_get_state(KEY_2) == KEY_SHORT_PRESS)
    {
        key_clear_state(KEY_2);
        return KEY_DOWN;
    }
	 if(key_get_state(KEY_2) == KEY_LONG_PRESS)
    {
        key_clear_state(KEY_2);
        return KEY_LONG_DOWN;
    }
	if(key_get_state(KEY_3) == KEY_LONG_PRESS)
    {
        key_clear_state(KEY_3);
        return  KEY_LONG_OK;
    }
    if(key_get_state(KEY_3) == KEY_SHORT_PRESS)
    {
        key_clear_state(KEY_3);
        return KEY_OK;
    }
    if(key_get_state(KEY_4) == KEY_SHORT_PRESS)
    {
        key_clear_state(KEY_4);
        return KEY_BACK;
    }
	  if(key_get_state(KEY_4) == KEY_LONG_PRESS)
    {
        key_clear_state(KEY_4);
        return KEY_LONG_BACK;
    }
	
    return KEY_NONE;
}
