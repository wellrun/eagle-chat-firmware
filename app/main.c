#define F_CPU	32000000L

#include "asf.h"
#include <avr/io.h>
#include "cdc.h"
#include <util/delay.h>
#include "tc.h"

#include <string.h>

uint16_t count = 4000;

// Configure timer overflow to happen every ~30 ms
#define TC_COUNT	4002
#define TC_CLK_DIV	TC_CLKSEL_DIV256_gc
#define APP_TC		TCC0

void app_step(void);

void app_step(void) {
	static uint32_t start = 0;
	if (start == 0) {
		start = rtc_get_time();
	}
	uint32_t now = rtc_get_time();
	cdc_log_int("Overflow. Time diff: ", now - start);

	/*
	if (now - start > 30) {
		--count;
		tc_write_period(&TCC0, count);
	} else if (now - start < 30) {
		++count;
		tc_write_period(&TCC0, count);
	}
	*/
	//cdc_log_int("Count: ", count);

	start = now;
}

int main(void);
int main ()
{
	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	rtc_init();

	cdc_start();

	while (!cdc_opened());
	rtc_set_time(0);
	cdc_log_int("Starting app", rtc_get_time());

	tc_enable(&APP_TC);

	tc_set_overflow_interrupt_callback(&APP_TC, app_step);

	tc_set_wgm(&APP_TC, TC_WG_NORMAL);
	tc_write_period(&APP_TC, TC_COUNT);

	tc_set_overflow_interrupt_level(&APP_TC, TC_INT_LVL_LO);

	tc_write_clock_source(&APP_TC, TC_CLK_DIV);


	while (1);
}
