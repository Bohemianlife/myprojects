#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <rthw.h>
#include <rtthread.h>
#include "LPC17xx.h"
#include "bdconfig.h"

#define BTN0_PRESSED					0x01
#define BTN1_PRESSED					0x02
#define BTN2_PRESSED					0x04
#define BTN3_PRESSED					0x08
#define BTN4_PRESSED					0x10
#define BTN5_PRESSED					0x20
#define BTN6_PRESSED					0x40

#define IO0_RAISING			    	0x0100
#define IO0_FALLING			    	0x0200
#define IO1_RAISING			    	0x0400
#define IO1_FALLING			    	0x0800
#define IO2_RAISING			    	0x1000
#define IO2_FALLING			    	0x2000

typedef struct{
	struct rt_device parent;
	const char* dev_name;
	short dev_flag;
	struct rt_event btnEvent;
}rt_button_lpc;

void rt_hw_button_init(void);

#endif	//_BUTTON_H_
