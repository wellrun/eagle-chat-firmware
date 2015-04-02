#include "system_timer.h"


void system_timer_init()
{
	tc_enable(&SYSTEM_TC);
	tc_set_wgm(&SYSTEM_TC, TC_WG_NORMAL);
	tc_write_period(&SYSTEM_TC, SYSTEM_TIMER_COUNT);
	tc_set_overflow_interrupt_level(&SYSTEM_TC, TC_INT_LVL_LO);
}

void system_timer_set_callback(void (*callback)(void))
{
	tc_set_overflow_interrupt_callback(&SYSTEM_TC, callback);
}

void system_timer_start()
{
	tc_write_clock_source(&SYSTEM_TC, SYSTEM_TIMER_DIV);
}
