#ifndef _MYBUTTON_H_
#define _MYBUTTON_H_

#include <rthw.h>
#include <rtthread.h>
#include "LPC17xx.h"
#include "button.h"
#include "stdbool.h"

void rt_hw_device_IO_init(void);

static void btn_thread_entry(void* parameter);
void rt_mybutton_init(void);

/*邮箱*/
int rt_mailbox_init(void);
static void mailbox_thread1_entry(void* parameter);
static void mailbox_thread2_entry(void* parameter);

/*消息队列*/
int rt_messagequeue_init(void);
static void msgque_thread1_entry(void* parameter);
static void msgque_thread2_entry(void* parameter);
static void msgque_thread3_entry(void* parameter);

/*信号量*/
int rt_semaphore_init(void);
static void sem_thread1_entry(void* parameter);

#endif //_MYBUTTON_H_
