#include "backlight.h"

sys_bl_t backlight_mode;                // 背光模式控制

/*
 * func     背光初始化
 * param    none
 * retval   none
 */
void backlight_init(kg_cfg_t cfg)
{
    backlight_mode = cfg.bl_level;
}

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


