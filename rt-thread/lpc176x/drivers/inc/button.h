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
 * ���ﶨ����һ���ṹ�����ͣ�rt_button_lpc�ǽṹ����
 * ��������ʱҪ��rt_button_lpc ������������
 */
typedef struct{
	struct rt_device parent;
	const char* dev_name;
	short dev_flag;
	struct rt_event btnEvent;
}rt_button_lpc;

void rt_hw_button_init(void);

#endif
