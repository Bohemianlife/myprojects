#include "myled.h"

#define rt_hw_led_on  LPC_GPIO2->FIOCLR|=0x2000;
#define rt_hw_led_off LPC_GPIO2->FIOSET|=0x2000;
//#define led1_on  LPC_GPIO1->FIOSET|=0x200000; //��
//#define led1_off LPC_GPIO1->FIOCLR|=0x200000; //��

/*�߳��õ���ջ������ARM��4�ֽڶ���ģ�����ջ�Ŀռ������4�ֽڶ����*/
static char led1_thread_stack[512];
/*��̬�̵߳��߳̿��ƿ�*/
static struct rt_thread led1_thread;

void static_thread_entry(void* parameter)
{
	while(1)
		{
			rt_hw_led_on;
			rt_thread_delay(10);
			rt_hw_led_off;
			rt_thread_delay(10);
			rt_kprintf("11111111\r\n");
		}
}

void dynamic_thread_entry(void* parameter)
{
	while(1)
		{
			rt_hw_led_on;
			rt_thread_delay(100);
			rt_hw_led_off;
			rt_thread_delay(100);
			rt_kprintf("22222222\r\n");
		}
}

void demo_thread_creat(void)
{
	rt_err_t result;
	/*��̬�̵߳��߳̿��ƿ�ָ��*/
	rt_thread_t led2_thread;
	
	LPC_GPIO2->FIODIR|=0x2000;
	
	/*������̬�̣߳����ȼ�20��ʱ��Ƭ2��ϵͳ�δ�*/
	result = rt_thread_init(&led1_thread,"led1",
													static_thread_entry,RT_NULL,
													(rt_uint8_t*)&led1_thread_stack[0],
													sizeof(led1_thread_stack),20,2);
	if(result == RT_EOK)
	{
		rt_thread_startup(&led1_thread);
	}
	
	/*������̬�̣߳���ջ��С512�ֽڣ����ȼ�21��ʱ��Ƭ4��ϵͳ�δ�*/
	led2_thread = rt_thread_create("led2",
																dynamic_thread_entry,RT_NULL,
																512,21,2);
	if(led2_thread != RT_NULL)
		rt_thread_startup(led2_thread);
}



///*�̵߳���ڵ㣬���߳��������������������￪ʼִ��*/
//static void led_thread_entry(void* parameter)
//{
//	LPC_GPIO2->FIODIR|=0x2000;//p2.13
////	LPC_GPIO1->FIODIR|=0x200000;
//	/*����߳���һ����Զѭ��ִ�е��߳�*/
//	while(1)
//	{
//		/*��LED,Ȼ����ʱ10��OS Tick*/
//		rt_hw_led_on;
//		rt_thread_delay(10);
////		rt_kprintf("11111111\r\n");
//		/*��LED,Ȼ����ʱ10��OS Tick*/
//		rt_hw_led_off;
//		rt_thread_delay(10);
////		rt_kprintf("22222222\r\n");
//	}
//}

//int rt_myledapplication_init()
//{
//	/*
//	 * ��ʼ��һ���߳�
//	 * �����ǡ�led��
//	 * ���λ���� led_thread_entry
//	 * ��ڲ����� RT_NULL,��������ᴫ�ݸ���ں����ģ�������һ��ָ���һ����
//	 * ���ȼ���25(AT91SAM7S64���õ�������ȼ�����32������ʹ��25)
//	 * ʱ��Ƭ��8(�������ͬ���ȼ����̴߳��ڣ�ʱ��Ƭ�Ż�����������)
//	 */
//	rt_thread_t tid1;
//	tid1 = rt_thread_create(
//			"led",
//			led_thread_entry,RT_NULL,
//			512,
//			25,8);
//		if(tid1 != RT_NULL)rt_thread_startup(tid1);
//	return 0;
//	/*
//	 * ��һ�������ǳ�ʼ��һ���̣߳�Ҳ����Ϊһ���̵߳�������׼����
//	 * ����������������߳�
//	 * 
//	 * ע������������������߳����̾�����������������������������
//	 * �̲߳ŵõ������ĵ��ȡ������ʱ���������Ѿ������ˣ���ô��ȡ����������
//	 * �����߳����ȼ��Ƿ���ڵ�ǰ�������ȼ���������ڣ�������ִ�����̡߳�
//	 */
//}

