#include "mybutton.h"


//#define rt_hw_led_on  LPC_GPIO1->FIOCLR|=0x200000;//P2.21
//#define rt_hw_led_off LPC_GPIO1->FIOSET|=0x200000;//P2.21

#define rt_hw_led_on  LPC_GPIO2->FIOCLR|=0x2000;//P2.13
#define rt_hw_led_off LPC_GPIO2->FIOSET|=0x2000;//P2.13

/*����*/
static struct rt_mailbox mb;
static char mb_pool[128];
static char mb_str1[] = "I'm a mail!";
static char mb_str2[] = "this is another mail!";
static char mb_str3[] = "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20";

/*��Ϣ����*/
static struct rt_messagequeue mq;
static char msgpool[2048];
char buf[] = "a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z";

void rt_hw_device_IO_init(void)
{
	//�ҵİ���
//	LPC_GPIO1->FIODIR|=0x200000;//p2.21����Ϊ���(��)
//	LPC_GPIO2->FIODIR&=~0x04;//p2.2����Ϊ����(����)
//	LPC_GPIO2->FIODIR&=~0x08;//p2.3����Ϊ����(����)
//	LPC_GPIOINT ->IO2IntEnF |= 0x04;
//	LPC_GPIOINT ->IO2IntEnF |= 0x08;
//	LPC_GPIOINT ->IO2IntEnR |= 0x04;
//	LPC_GPIOINT ->IO2IntEnR |= 0x08;
	
	//JZ����
	LPC_GPIO2->FIODIR|=0x2000;//p2.13����Ϊ���(��)
	LPC_GPIO2->FIODIR&=~0x10;//p2.4����Ϊ����(����)
	LPC_GPIO2->FIODIR&=~0x20;//p2.5����Ϊ����(����)
	LPC_GPIO2->FIODIR&=~0x40;//p2.6����Ϊ����(����)
	LPC_GPIOINT ->IO2IntEnF |= 0x10;
	LPC_GPIOINT ->IO2IntEnF |= 0x20;
	LPC_GPIOINT ->IO2IntEnF |= 0x40;
	LPC_GPIOINT ->IO2IntEnR |= 0x10;
	LPC_GPIOINT ->IO2IntEnR |= 0x20;
	LPC_GPIOINT ->IO2IntEnR |= 0x40;
}

/*�̵߳���ڵ㣬���߳��������������������￪ʼִ��*/
static void btn_thread_entry(void* parameter)
{
	rt_device_t device;
	extern rt_button_lpc btn_device;
	rt_uint32_t e;
	rt_hw_led_on;//����
	
	device = rt_device_find("button"); //�����豸
	if(device!= RT_NULL)
	{
		rt_device_open(device, RT_DEVICE_OFLAG_RDONLY); //���豸
	}
	while(1)
	{
		if(rt_event_recv(&(btn_device.btnEvent), (0x10|0x20|0x40), RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
       RT_WAITING_FOREVER, &e)==RT_EOK)
		{
			rt_kprintf("receive succeed 0x%x\r\n", e);
			/*������������*/
			rt_mb_send(&mb, (rt_uint32_t)&mb_str3[0]);
			rt_kprintf("I have already send a mail successful!\r\n");
			/*����������Ϣ����*/
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

/*��������*/
static void mailbox_thread2_entry(void* parameter)
{
	rt_uint8_t count = 0;
//	while(1)
//	{
		rt_enter_critical();
		count++;
		if(count & 0x01)
		{/*����mb_str1��ַ��������*/
			rt_mb_send(&mb, (rt_uint32_t)&mb_str1[0]);
		}
		else
		{/*����mb_str2��ַ��������*/
			rt_mb_send(&mb, (rt_uint32_t)&mb_str2[0]);
		}
		rt_thread_delay(50);
		rt_exit_critical();
//	}
}

/*
 * ��������
 * "(rt_uint32_t*)&str"
 * ���:�� 8bit unsigned integer typeǿ��ת���� 32bit unsigned integer type
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

/*�����ʼ��*/
int rt_mailbox_init()
{
	rt_thread_t tid1;
	rt_thread_t tid2;
	/*������һ����̬����*/
	rt_mb_init(&mb,"mbt",&mb_pool[0],sizeof(mb_pool),RT_IPC_FLAG_FIFO);
	/*����������̬����*/
	tid1 = rt_thread_create("t1",mailbox_thread1_entry,RT_NULL,512,24,8);
		if(tid1 != RT_NULL)rt_thread_startup(tid1);	
	tid2 = rt_thread_create("t2",mailbox_thread2_entry,RT_NULL,512,25,8);
		if(tid2 != RT_NULL)rt_thread_startup(tid2);
	return 0;
}

/*������Ϣ����(ֻ����һ��)*/
static void msgque_thread2_entry(void* parameter)
{
	int i, result;
	char buf[] = "this is message No.x";
	rt_enter_critical();//����������
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
	rt_exit_critical();//����������
//}
}

/*���ͽ�����Ϣ����(ֻ����һ��)*/
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

/*������Ϣ����(���޴ν���)*/
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

/*��Ϣ���г�ʼ��*/
int rt_messagequeue_init()
{
	rt_thread_t tid1;
	rt_thread_t tid2;
	rt_thread_t tid3;
	/*��ʼ����Ϣ����*/
	 rt_mq_init(&mq,"mqt",&msgpool[0],128-sizeof(void*),sizeof(msgpool),RT_IPC_FLAG_FIFO);
	/*������̬��Ϣ����*/
	tid1 = rt_thread_create("mq1",msgque_thread1_entry,RT_NULL,512,20,8);
		if(tid1 != RT_NULL)rt_thread_startup(tid1);	
	tid2 = rt_thread_create("mq2",msgque_thread2_entry,RT_NULL,512,21,8);
		if(tid2 != RT_NULL)rt_thread_startup(tid2);
	tid3 = rt_thread_create("mq3",msgque_thread3_entry,RT_NULL,512,22,8);
		if(tid3 != RT_NULL)rt_thread_startup(tid3);
	return 0;	
}

/*�ź���*/
static rt_sem_t sem = RT_NULL;//ָ���ź�����ָ��(��̬)
static struct rt_semaphore sem1;//�ź������ƿ�(��̬)

/*�ź���*/
static void sem_thread1_entry(void* parameter)
{
	rt_err_t result;
	rt_enter_critical();//����������
	result = rt_sem_take(sem, 10);//��̬��ȡ�ź���
	if (result == -RT_ETIMEOUT)
	{
		rt_kprintf("dynamic take semaphore timeout\n");//��ʱ
	}
	result = rt_sem_take(&sem1, 10);//��̬��ȡ�ź���
	if (result == -RT_ETIMEOUT)
	{
		rt_kprintf("static take semaphore timeout\n");//��ʱ
	}
	rt_sem_release(sem);//��̬�ͷ��ź���
	rt_kprintf("dynamic release semaphore\n");
	rt_sem_release(&sem1);//��̬�ͷ��ź���
	rt_kprintf("static release semaphore\n");
	rt_thread_delay(200);
	result = rt_sem_take(sem, RT_WAITING_FOREVER);//��̬��ȡ�ź���
	if(result==RT_EOK)
	{
		rt_kprintf("I have already take semaphore dynamic!\n");
	}
	else
	{
		rt_kprintf("no take semaphore!\n");
	}
	result = rt_sem_take(&sem1, RT_WAITING_FOREVER);//��̬��ȡ�ź���
	if(result==RT_EOK)
	{
		rt_kprintf("I have already take semaphore static!\n");
	}
	else
	{
		rt_kprintf("no take semaphore!\n");
	}
	rt_sem_release(sem);//��̬�ͷ��ź���
	rt_kprintf("dynamic release semaphore too\n");
	rt_sem_release(&sem1);//��̬�ͷ��ź���
	rt_kprintf("static release semaphore too\n");
	rt_exit_critical();//����������
}

int rt_semaphore_init()
{
	rt_thread_t tid1;
	/*��ʼ����̬�ź���*/
	rt_sem_init (&sem1, "sem1",0, RT_IPC_FLAG_FIFO);
	/*������̬�ź���*/
	sem = rt_sem_create ("sem", 2,RT_IPC_FLAG_FIFO);
	/*�����߳�*/
	tid1 = rt_thread_create("sem1",sem_thread1_entry,RT_NULL,512,23,8);
		if(tid1 != RT_NULL)rt_thread_startup(tid1);
	return 0;
}


/*������*/
int rt_mutex_sem_init()
{
	/*������̬������*/
	mutex = rt_mutex_create("mutex", RT_IPC_FLAG_FIFO));
}


