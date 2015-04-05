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

/*����*/
int rt_mailbox_init(void);
static void mailbox_thread1_entry(void* parameter);
static void mailbox_thread2_entry(void* parameter);

/*��Ϣ����*/
int rt_messagequeue_init(void);
static void msgque_thread1_entry(void* parameter);
static void msgque_thread2_entry(void* parameter);
static void msgque_thread3_entry(void* parameter);

/*�ź���*/
int rt_semaphore_init(void);
static void sem_thread1_entry(void* parameter);

#endif //_MYBUTTON_H_
