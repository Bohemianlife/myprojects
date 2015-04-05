#include "button.h"

#define rt_hw_led_on  LPC_GPIO2->FIOCLR|=0x2000;
#define rt_hw_led_off LPC_GPIO2->FIOSET|=0x2000;

//register addr for button pin
rt_uint32_t IO_BtnAddr[3] = {0x10,0x20,0x40};

#if (defined(RT_USING_BTN0) || defined(RT_USING_BTN1) || defined(RT_USING_BTN2)) && defined(RT_USING_DEVICE)

rt_button_lpc btn_device;
const char* btnName = "button";

void EINT3_IRQHandler(void) __irq
{
	
	if((LPC_GPIOINT->IO2IntStatR & IO_BtnAddr[0]))
	{
		rt_event_send(&(btn_device.btnEvent), 0x10);
		rt_kprintf("btn0 = 0x10\r\n");
		LPC_GPIOINT ->IO2IntClr |= IO_BtnAddr[0];
	}
	if((LPC_GPIOINT->IO2IntStatR & IO_BtnAddr[1]))
	{
		rt_event_send(&(btn_device.btnEvent), 0x20);
		rt_kprintf("btn1 = 0x20\r\n");
		LPC_GPIOINT ->IO2IntClr |= IO_BtnAddr[1];
	}
	if((LPC_GPIOINT->IO2IntStatR & IO_BtnAddr[2]))
	{
		rt_event_send(&(btn_device.btnEvent), 0x40);
		rt_kprintf("btn3 = 0x40\r\n");
		LPC_GPIOINT ->IO2IntClr |= IO_BtnAddr[2];
	}
	
}

rt_event_t rt_event_create (const char* name, rt_uint8_t flag);

static rt_err_t rt_btn_init (rt_device_t dev)
{
/*初始化事件rt_event_init(事件对象的句柄,事件名称,事件标志)*/
	rt_event_init(&(btn_device.btnEvent), "btn_event", RT_IPC_FLAG_FIFO);
	return RT_EOK;
}

static rt_err_t rt_btn_open(rt_device_t dev, rt_uint16_t oflag)
{
	RT_ASSERT(dev != RT_NULL);
	NVIC_EnableIRQ(EINT3_IRQn);//打开相关IO中断
	return RT_EOK;
}

static rt_err_t rt_btn_close(rt_device_t dev)
{
	RT_ASSERT(dev != RT_NULL);
	NVIC_DisableIRQ(EINT3_IRQn);//关闭相关IO中断
	return RT_EOK;
}

void rt_hw_button_init(void)
{
	rt_button_lpc* btn;
	/* get button device */
	btn = &btn_device;
	btn->dev_name = btnName;
	btn->dev_flag = RT_DEVICE_FLAG_RDONLY;
	
	/* device initialization */
	btn->parent.type = RT_Device_Class_Char;
	
	/* device interface */
	btn->parent.init       =rt_btn_init;
	btn->parent.open       =rt_btn_open;
	btn->parent.close      =rt_btn_close;
	btn->parent.read       =RT_NULL;
	btn->parent.write      =RT_NULL;
	btn->parent.control    =RT_NULL;
	btn->parent.user_data  =RT_NULL; 
	
	if(rt_device_register(&btn->parent, btn->dev_name, btn->dev_flag) == RT_EOK){
		rt_kprintf("System Message: %s registered ok! <---> in funciton -> %s(), at Line %d \n", btn->dev_name, __FUNCTION__, __LINE__);
	}
}

#endif

