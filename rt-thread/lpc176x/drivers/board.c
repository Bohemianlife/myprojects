/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 * 2010-02-04     Magicoe      ported to LPC17xx
 * 2010-05-02     Aozima       update CMSIS to 130
 */

#include <rthw.h>
#include <rtthread.h>

#include "uart.h"
#include "board.h"
#include "LPC17xx.h"
#include "button.h"
#include "mybutton.h"

/**
 * @addtogroup LPC17xx
 */

/*@{*/

/**
 * This is the timer interrupt service routine.
 *
 */
void rt_hw_timer_handler(void)
{
	/* enter interrupt */
	rt_interrupt_enter();

	rt_tick_increase();

	/* leave interrupt */
	rt_interrupt_leave();
}

void SysTick_Handler(void)
{
    rt_hw_timer_handler();
}

/**
 * This function will initial LPC17xx board.
 */
void rt_hw_board_init()
{
	rt_hw_device_IO_init();
	/* NVIC Configuration */
#define NVIC_VTOR_MASK              0x3FFFFF80
#ifdef  VECT_TAB_RAM
	/* Set the Vector Table base location at 0x10000000 */
	SCB->VTOR  = (0x10000000 & NVIC_VTOR_MASK);
#else  /* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x00000000 */
	SCB->VTOR  = (0x00000000 & NVIC_VTOR_MASK);
#endif

	/* initialize systick */
	SysTick_Config( SystemCoreClock/RT_TICK_PER_SECOND);
	/* set pend exception priority */
	NVIC_SetPriority(PendSV_IRQn, (1<<__NVIC_PRIO_BITS) - 1);

#ifdef RT_USING_UART0
	rt_hw_uart_init();
	rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

#ifdef RT_USING_UART3
	rt_hw_uart_init();
	rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

rt_hw_button_init();
}

/*@}*/