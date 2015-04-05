/**
 * File      : button.c
 * This file is part of RTT_Application
 * The way we handle the button's interrupt is
 * like the Linux, but still need to implement
 * a timer to handle the whipping problem of button
 * COPYRIGHT (C) 2014 KJ Team
 *
 * Change Logs:
 * Date           Author       Notes
 * 2014-10-10     KevinJZ      first implementation
 */

#include "button.h"
//register addr for button pin
rt_uint32_t IO_BtnAddr[8] = {0};

#if (defined(BD_USING_BTN0) || defined(BD_USING_BTN1) || defined(BD_USING_BTN2))&& defined(RT_USING_DEVICE)

	rt_button_lpc btn_device;
	const char* btnName = "button";


void EINT3_IRQHandler(void) __irq
{
	unsigned int tempIntStat0F = 0;
	unsigned int tempIntStat2F = 0;
  unsigned int tempIntStat0R = 0;
	unsigned int tempIntStat2R = 0;
	rt_uint32_t btnEvtVal = 0;
	tempIntStat0F = LPC_GPIOINT->IO0IntStatF;
	tempIntStat2F = LPC_GPIOINT->IO2IntStatF;
  tempIntStat0R = LPC_GPIOINT->IO0IntStatR;
	tempIntStat2R = LPC_GPIOINT->IO2IntStatR;
	btnEvtVal = 0;

	#ifdef BD_USING_BTN0
		if(tempIntStat2F & IO_BtnAddr[0])
		{
			btnEvtVal |= BTN0_PRESSED;
			LPC_GPIOINT->IO2IntClr |= IO_BtnAddr[0];
			//rt_kprintf("btn0\n");
		}
	#endif

	#ifdef BD_USING_BTN1
		if(tempIntStat2F & IO_BtnAddr[1])
		{
			btnEvtVal |= BTN1_PRESSED;
			LPC_GPIOINT->IO2IntClr |= IO_BtnAddr[1];
			//rt_kprintf("btn1\n");
		}
	#endif

  #ifdef BD_USING_BTN2
		if(tempIntStat2F & IO_BtnAddr[2])
		{
			btnEvtVal |= BTN2_PRESSED;
			LPC_GPIOINT->IO2IntClr |= IO_BtnAddr[2];
		}
	#endif

   //-------------------------------------------------------------
	if((btnEvtVal & (BTN0_PRESSED|BTN1_PRESSED|BTN2_PRESSED)) != 0){
		rt_event_send(&(btn_device.btnEvent), (btnEvtVal & (BTN0_PRESSED|BTN1_PRESSED|BTN2_PRESSED)));
	}

  #ifdef BD_USING_ANGLE_ENC
    if(tempIntStat0F & IO_BtnAddr[3]) 																	//INX_0
		{
			btnEvtVal |= BTN3_PRESSED;
			LPC_GPIOINT->IO0IntClr |= IO_BtnAddr[3];
		}
    if(tempIntStat0F & IO_BtnAddr[4]) 																	//INX_1
		{
			btnEvtVal |= BTN4_PRESSED;
			LPC_GPIOINT->IO0IntClr |= IO_BtnAddr[4];
		}
    if(tempIntStat0F & IO_BtnAddr[5]) 																	//INX_2
		{
			btnEvtVal |= BTN5_PRESSED;
			LPC_GPIOINT->IO0IntClr |= IO_BtnAddr[5];
		}
    if((btnEvtVal & (BTN3_PRESSED|BTN4_PRESSED|BTN5_PRESSED)) != 0){
      rt_event_send(&(btn_device.btnEvent), (btnEvtVal & (BTN3_PRESSED|BTN4_PRESSED|BTN5_PRESSED)));
    }
  #endif

  #ifdef BD_USING_DOOR_CTRL
    if(tempIntStat0R & IO_BtnAddr[3]) 																	//INX_0
    {
			btnEvtVal |= IO0_RAISING;
			LPC_GPIOINT->IO0IntClr |= IO_BtnAddr[3];
		}
    else if(tempIntStat0F & IO_BtnAddr[3])
    {
      btnEvtVal |= IO0_FALLING;
			LPC_GPIOINT->IO0IntClr |= IO_BtnAddr[3];
    }

    if(tempIntStat0R & IO_BtnAddr[4]) 																	//INX_1
		{
			btnEvtVal |= IO1_RAISING;
			LPC_GPIOINT->IO0IntClr |= IO_BtnAddr[4];
		}
    else if(tempIntStat0F & IO_BtnAddr[4])
    {
      btnEvtVal |= IO1_FALLING;
			LPC_GPIOINT->IO0IntClr |= IO_BtnAddr[4];
    }

    if(tempIntStat0R & IO_BtnAddr[5]) 																	//INX_2
		{
			btnEvtVal |= IO2_RAISING;
			LPC_GPIOINT->IO0IntClr |= IO_BtnAddr[5];
		}
    else if(tempIntStat0F & IO_BtnAddr[5])
    {
      btnEvtVal |= IO2_FALLING;
			LPC_GPIOINT->IO0IntClr |= IO_BtnAddr[5];
    }

    if((btnEvtVal & (IO0_RAISING|IO0_FALLING|IO1_RAISING|IO1_FALLING|IO2_RAISING|IO2_FALLING)) != 0){
      rt_event_send(&(btn_device.btnEvent), (btnEvtVal & (IO0_RAISING|IO0_FALLING|IO1_RAISING|IO1_FALLING|IO2_RAISING|IO2_FALLING)));
    }
  #endif


	btnEvtVal = 0;


}
//--------------------------------------------

static rt_err_t rt_btn_init (rt_device_t dev){
	rt_event_init(&(btn_device.btnEvent), "btn_event", RT_IPC_FLAG_FIFO);
	return RT_EOK;
}

static rt_err_t rt_btn_open(rt_device_t dev, rt_uint16_t oflag)
{
	RT_ASSERT(dev != RT_NULL);

	return RT_EOK;

}

static rt_err_t rt_btn_close(rt_device_t dev)
{
	RT_ASSERT(dev != RT_NULL);
	//关闭相关IO中断
	return RT_EOK;
}

void rt_hw_button_init()
{
	rt_button_lpc* btn;
	/* get led device */

	btn = &btn_device;
	btn->dev_name = btnName;
	btn->dev_flag = RT_DEVICE_FLAG_RDONLY;

	/* device initialization */
	btn->parent.type = RT_Device_Class_Char;

	/* device interface */
	btn->parent.init 	    = rt_btn_init;
	btn->parent.open 	    = rt_btn_open;
	btn->parent.close     = rt_btn_close;
	btn->parent.read 	    = RT_NULL;
	btn->parent.write     = RT_NULL;
	btn->parent.control   = RT_NULL;
	btn->parent.user_data = RT_NULL;

	if(rt_device_register(&btn->parent, btn->dev_name, btn->dev_flag) == RT_EOK){
		rt_kprintf("System Message: %s registered ok! <---> in funciton -> %s(), at Line %d \n", btn->dev_name, __FUNCTION__, __LINE__);
	}

}
#endif
