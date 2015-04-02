#define F_CPU   32000000L

#include <string.h>

#include <avr/io.h>
#include <util/delay.h>

#include "asf.h"
#include "cdc.h"
#include "system_timer.h"
#include "stack/stack.h"

volatile bool quit = false;

void app_step(void);

void app_step(void)
{
	static uint32_t start = 0;
	static uint16_t count = 0;

	if (start == 0)
		start = rtc_get_time();
	uint32_t now = rtc_get_time();
	cdc_log_int("Overflow. Time diff: ", now - start);
	start = now;

	if (count == 67) { // about two seconds will have elapsed
		quit = true;
	}
	count++;
}

int main(void);
int main()
{
	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	rtc_init();

	cdc_start();

	while (!cdc_opened());
	rtc_set_time(0);
	cdc_log_int("Starting app", rtc_get_time());

	system_timer_init();
	system_timer_set_callback(app_step);
	system_timer_start();

	while (!quit);

	cdc_log_int("Unused stack bytes: ", stack_count());

	while (1);
}
