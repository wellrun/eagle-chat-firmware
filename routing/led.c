#include "app/asf.h"

#include "user_board.h"

#include "led.h"

static void callback();

void led_init()
{
	tc_enable(&LED_TIMER);
	tc_set_wgm(&LED_TIMER, TC_WG_NORMAL);
	tc_write_period(&LED_TIMER, LED_TIMER_COUNT);
	tc_set_overflow_interrupt_level(&LED_TIMER, TC_INT_LVL_LO);

	tc_set_overflow_interrupt_callback(&LED_TIMER, callback);

	ioport_set_pin_dir(DEBUG_LED_pin, IOPORT_DIR_OUTPUT);
	ioport_set_pin_low(DEBUG_LED_pin);
}

void led_on()
{
	ioport_set_pin_high(DEBUG_LED_pin);
	tc_write_clock_source(&LED_TIMER, LED_TIMER_DIV);
}

void callback() {
	tc_write_clock_source(&LED_TIMER, TC_CLKSEL_OFF_gc);
	ioport_set_pin_low(DEBUG_LED_pin);
}
