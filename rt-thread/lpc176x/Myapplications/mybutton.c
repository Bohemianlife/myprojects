#include "mybutton.h"


//#define rt_hw_led_on  LPC_GPIO1->FIOCLR|=0x200000;//P2.21
//#define rt_hw_led_off LPC_GPIO1->FIOSET|=0x200000;//P2.21

#define rt_hw_led_on  LPC_GPIO2->FIOCLR|=0x2000;//P2.13
#define rt_hw_led_off LPC_GPIO2->FIOSET|=0x2000;//P2.13

/*邮箱*/
static struct rt_mailbox mb;
static char mb_pool[128];
static char mb_str1[] = "I'm a mail!";
static char mb_str2[] = "this is another mail!";
static char mb_str3[] = "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20";

/*消息队列*/
static struct rt_messagequeue mq;
static char msgpool[2048];
char buf[] = "a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z";

void rt_hw_device_IO_init(void)
{
	//我的板子
//	LPC_GPIO1->FIODIR|=0x200000;//p2.21设置为输出(灯)
//	LPC_GPIO2->FIODIR&=~0x04;//p2.2设置为输入(按键)
//	LPC_GPIO2->FIODIR&=~0x08;//p2.3设置为输入(按键)
//	LPC_GPIOINT ->IO2IntEnF |= 0x04;
//	LPC_GPIOINT ->IO2IntEnF |= 0x08;
//	LPC_GPIOINT ->IO2IntEnR |= 0x04;
//	LPC_GPIOINT ->IO2IntEnR |= 0x08;
	
	//JZ板子
	LPC_GPIO2->FIODIR|=0x2000;//p2.13设置为输出(灯)
	LPC_GPIO2->FIODIR&=~0x10;//p2.4设置为输入(按键)
	LPC_GPIO2->FIODIR&=~0x20;//p2.5设置为输入(按键)
	LPC_GPIO2->FIODIR&=~0x40;//p2.6设置为输入(按键)
	LPC_GPIOINT ->IO2IntEnF |= 0x10;
	LPC_GPIOINT ->IO2IntEnF |= 0x20;
	LPC_GPIOINT ->IO2IntEnF |= 0x40;
	LPC_GPIOINT ->IO2IntEnR |= 0x10;
	LPC_GPIOINT ->IO2IntEnR |= 0x20;
	LPC_GPIOINT ->IO2IntEnR |= 0x40;
}

/*线程的入口点，当线程运行起来后，它将从这里开始执行*/
static void btn_thread_entry(void* parameter)
{
	rt_device_t device;
	extern rt_button_lpc btn_device;
	rt_uint32_t e;
	rt_hw_led_on;//灯亮
	
	device = rt_device_find("button"); //查找设备
	if(device!= RT_NULL)
	{
		rt_device_open(device, RT_DEVICE_OFLAG_RDONLY); //打开设备
	}
	while(1)
	{
		if(rt_event_recv(&(btn_device.btnEvent), (0x10|0x20|0x40), RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
       RT_WAITING_FOREVER, &e)==RT_EOK)
		{
			rt_kprintf("receive succeed 0x%x\r\n", e);
			/*按键发送邮箱*/
			rt_mb_send(&mb, (rt_uint32_t)&mb_str3[0]);
			rt_kprintf("I have already send a mail successful!\r\n");
			/*按键发送消息队列*/
			rt_mq_send(&mq, &buf[0], sizeof(buf));
			rt_kprintf("I have already send a message queue successful!\r\n");
		}
		rt_thread_delay(10);
	}
}

void rt_mybutton_init()
{
	rt_thread_t tid;
	tid = rt_thread_create(
			"button",
			btn_thread_entry,RT_NULL,
			512,
			25,8);
		if(tid != RT_NULL)rt_thread_startup(tid);
}

/*发送邮箱*/
static void mailbox_thread2_entry(void* parameter)
{
	rt_uint8_t count = 0;
//	while(1)
//	{
		rt_enter_critical();
		count++;
		if(count & 0x01)
		{/*发送mb_str1地址到邮箱中*/
			rt_mb_send(&mb, (rt_uint32_t)&mb_str1[0]);
		}
		else
		{/*发送mb_str2地址到邮箱中*/
			rt_mb_send(&mb, (rt_uint32_t)&mb_str2[0]);
		}
		rt_thread_delay(50);
		rt_exit_critical();
//	}
}

/*
 * 接收邮箱
 * "(rt_uint32_t*)&str"
 * 理解:将 8bit unsigned integer type强制转换成 32bit unsigned integer type
 */
static void mailbox_thread1_entry(void* parameter)
{
	unsigned char *str;
	while(1)
	{
		if(rt_mb_recv(&mb,(rt_uint32_t*)&str,RT_WAITING_FOREVER)==RT_EOK)
		{
			rt_kprintf("mailbox_thread1:I have already get a mail, the content:%s\r\n", str);
			rt_thread_delay(10);
		}
	}
}

/*邮箱初始化*/
int rt_mailbox_init()
{
	rt_thread_t tid1;
	rt_thread_t tid2;
	/*创建了一个静态邮箱*/
	rt_mb_init(&mb,"mbt",&mb_pool[0],sizeof(mb_pool),RT_IPC_FLAG_FIFO);
	/*创建两个动态邮箱*/
	tid1 = rt_thread_create("t1",mailbox_thread1_entry,RT_NULL,512,24,8);
		if(tid1 != RT_NULL)rt_thread_startup(tid1);	
	tid2 = rt_thread_create("t2",mailbox_thread2_entry,RT_NULL,512,25,8);
		if(tid2 != RT_NULL)rt_thread_startup(tid2);
	return 0;
}

/*发送消息队列(只发送一次)*/
static void msgque_thread2_entry(void* parameter)
{
	int i, result;
	char buf[] = "this is message No.x";
	rt_enter_critical();//调度器上锁
	rt_kprintf("\r\n");
//	while(1)
//	{
		for(i=0;i<10;i++)
		{
			buf[sizeof(buf)-2] = '0' + i;
			rt_kprintf("msgque_thread2: send message - %s\n", buf);
			result = rt_mq_send(&mq, &buf[0], sizeof(buf));
			if ( result == -RT_EFULL)
			{
				rt_kprintf("message queue full, delay 1s\n");
				rt_thread_delay(100);
			}
		}
		rt_thread_delay(10);
	rt_exit_critical();//调度器解锁
//}
}

/*发送紧急消息队列(只发送一次)*/
static void msgque_thread3_entry(void* parameter)
{
	char buf[] = "this is an urgent message!";
//	while (1)
//		{
			rt_kprintf("\r\n");
			rt_kprintf("msgque_thread3: send an urgent message\n");
			rt_mq_urgent(&mq, &buf[0], sizeof(buf));
			rt_kprintf("msgque_thread3:I recv a message is: %s\n", buf);
			rt_thread_delay(25);
//		}
}

/*接收消息队列(无限次接收)*/
static void msgque_thread1_entry(void* parameter)
{
	char buf[128];
	rt_kprintf("\r\n");
	while(1)
		{
			rt_memset(&buf[0], 0, sizeof(buf));
			if(rt_mq_recv(&mq,&buf[0],sizeof(buf),RT_WAITING_FOREVER)==RT_EOK)
				{
					rt_kprintf("msgque_thread1: recv a msg, the content:%s\n", buf);
					rt_thread_delay(10);
				}
		}
}

/*消息队列初始化*/
int rt_messagequeue_init()
{
	rt_thread_t tid1;
	rt_thread_t tid2;
	rt_thread_t tid3;
	/*初始化消息队列*/
	 rt_mq_init(&mq,"mqt",&msgpool[0],128-sizeof(void*),sizeof(msgpool),RT_IPC_FLAG_FIFO);
	/*创建动态消息队列*/
	tid1 = rt_thread_create("mq1",msgque_thread1_entry,RT_NULL,512,20,8);
		if(tid1 != RT_NULL)rt_thread_startup(tid1);	
	tid2 = rt_thread_create("mq2",msgque_thread2_entry,RT_NULL,512,21,8);
		if(tid2 != RT_NULL)rt_thread_startup(tid2);
	tid3 = rt_thread_create("mq3",msgque_thread3_entry,RT_NULL,512,22,8);
		if(tid3 != RT_NULL)rt_thread_startup(tid3);
	return 0;	
}

/*信号量*/
static rt_sem_t sem = RT_NULL;//指向信号量的指针(动态)
static struct rt_semaphore sem1;//信号量控制块(静态)

/*信号量*/
static void sem_thread1_entry(void* parameter)
{
	rt_err_t result;
	rt_enter_critical();//调度器上锁
	result = rt_sem_take(sem, 10);//动态获取信号量
	if (result == -RT_ETIMEOUT)
	{
		rt_kprintf("dynamic take semaphore timeout\n");//超时
	}
	result = rt_sem_take(&sem1, 10);//静态获取信号量
	if (result == -RT_ETIMEOUT)
	{
		rt_kprintf("static take semaphore timeout\n");//超时
	}
	rt_sem_release(sem);//动态释放信号量
	rt_kprintf("dynamic release semaphore\n");
	rt_sem_release(&sem1);//静态释放信号量
	rt_kprintf("static release semaphore\n");
	rt_thread_delay(200);
	result = rt_sem_take(sem, RT_WAITING_FOREVER);//动态获取信号量
	if(result==RT_EOK)
	{
		rt_kprintf("I have already take semaphore dynamic!\n");
	}
	else
	{
		rt_kprintf("no take semaphore!\n");
	}
	result = rt_sem_take(&sem1, RT_WAITING_FOREVER);//静态获取信号量
	if(result==RT_EOK)
	{
		rt_kprintf("I have already take semaphore static!\n");
	}
	else
	{
		rt_kprintf("no take semaphore!\n");
	}
	rt_sem_release(sem);//动态释放信号量
	rt_kprintf("dynamic release semaphore too\n");
	rt_sem_release(&sem1);//静态释放信号量
	rt_kprintf("static release semaphore too\n");
	rt_exit_critical();//调度器解锁
}

int rt_semaphore_init()
{
	rt_thread_t tid1;
	/*初始化静态信号量*/
	rt_sem_init (&sem1, "sem1",0, RT_IPC_FLAG_FIFO);
	/*创建动态信号量*/
	sem = rt_sem_create ("sem", 2,RT_IPC_FLAG_FIFO);
	/*创建线程*/
	tid1 = rt_thread_create("sem1",sem_thread1_entry,RT_NULL,512,23,8);
		if(tid1 != RT_NULL)rt_thread_startup(tid1);
	return 0;
}


/*互斥量*/
int rt_mutex_sem_init()
{
	/*创建动态互斥量*/
	mutex = rt_mutex_create("mutex", RT_IPC_FLAG_FIFO));
}


