#ifndef _MYLED_H_
#define _MYLED_H_

#include <rthw.h>
#include <rtthread.h>
#include "LPC17xx.h"

//int rt_myledapplication_init();
static void led_thread_entry(void* parameter);

void static_thread_entry(void* parameter);
void dynamic_thread_entry(void* parameter);
void demo_thread_creat(void);

#endif
