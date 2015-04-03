#include "asf.h"
#include "cdc.h"

int main (void)
{
	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	cdc_start();

	while (1);

}
