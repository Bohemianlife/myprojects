#include "myled.h"

#define rt_hw_led_on  LPC_GPIO2->FIOCLR|=0x2000;
#define rt_hw_led_off LPC_GPIO2->FIOSET|=0x2000;
//#define led1_on  LPC_GPIO1->FIOSET|=0x200000; //亮
//#define led1_off LPC_GPIO1->FIOCLR|=0x200000; //灭

/*线程用到的栈，由于ARM是4字节对齐的，所以栈的空间必须是4字节对齐的*/
static char led1_thread_stack[512];
/*静态线程的线程控制块*/
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
	/*动态线程的线程控制块指针*/
	rt_thread_t led2_thread;
	
	LPC_GPIO2->FIODIR|=0x2000;
	
	/*创建动态线程：优先级20，时间片2个系统滴答*/
	result = rt_thread_init(&led1_thread,"led1",
													static_thread_entry,RT_NULL,
													(rt_uint8_t*)&led1_thread_stack[0],
													sizeof(led1_thread_stack),20,2);
	if(result == RT_EOK)
	{
		rt_thread_startup(&led1_thread);
	}
	
	/*创建动态线程：堆栈大小512字节，优先级21，时间片4个系统滴答*/
	led2_thread = rt_thread_create("led2",
																dynamic_thread_entry,RT_NULL,
																512,21,2);
	if(led2_thread != RT_NULL)
		rt_thread_startup(led2_thread);
}



///*线程的入口点，当线程运行起来后，它将从这里开始执行*/
//static void led_thread_entry(void* parameter)
//{
//	LPC_GPIO2->FIODIR|=0x2000;//p2.13
////	LPC_GPIO1->FIODIR|=0x200000;
//	/*这个线程是一个永远循环执行的线程*/
//	while(1)
//	{
//		/*开LED,然后延时10个OS Tick*/
//		rt_hw_led_on;
//		rt_thread_delay(10);
////		rt_kprintf("11111111\r\n");
//		/*关LED,然后延时10个OS Tick*/
//		rt_hw_led_off;
//		rt_thread_delay(10);
////		rt_kprintf("22222222\r\n");
//	}
//}

//int rt_myledapplication_init()
//{
//	/*
//	 * 初始化一个线程
//	 * 名称是‘led’
//	 * 入口位置是 led_thread_entry
//	 * 入口参数是 RT_NULL,这个参数会传递给入口函数的，可以是一个指针或一个数
//	 * 优先级是25(AT91SAM7S64配置的最大优先级数是32，这里使用25)
//	 * 时间片是8(如果有相同优先级的线程存在，时间片才会真正起作用)
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
//	 * 上一步仅仅是初始化一个线程，也就是为一个线程的运行做准备，
//	 * 这里则是启动这个线程
//	 * 
//	 * 注：这个函数并不代表线程立刻就运行起来，当调度器启动起来后，
//	 * 线程才得到真正的调度。如果此时，调度器已经运行了，那么则取决于新启动
//	 * 动的线程优先级是否高于当前任务优先级，如果高于，则立刻执行新线程。
//	 */
//}

