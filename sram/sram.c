#include "sram.h"
#include <user_board.h>
#include "asf.h"

void sram_init(void) {
	ioport_set_pin_dir(SRAM_CS_pin, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(SRAM_MOSI_pin, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(SRAM_MISO_pin, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(SRAM_SCK_pin, IOPORT_DIR_OUTPUT);
}