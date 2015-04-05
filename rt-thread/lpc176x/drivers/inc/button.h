#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <rthw.h>
#include <rtthread.h>
#include "LPC17xx.h"
//#include "bdconfig.h"

#define RT_USING_BTN0
#define RT_USING_BTN1
#define RT_USING_BTN2

/*
 * 这里定义了一个结构体类型，rt_button_lpc是结构变量
 * 声明变量时要用rt_button_lpc 来声明变量。
 */
typedef struct{
	struct rt_device parent;
	const char* dev_name;
	short dev_flag;
	struct rt_event btnEvent;
}rt_button_lpc;

void rt_hw_button_init(void);

#endif
