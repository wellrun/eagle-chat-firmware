#include "board.h"
#include "asf.h"

void sha204_board_init(void) {
	ioport_init();

	sha204p_init();

	ioport_set_pin_dir(ATSHA_SCK, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(ATSHA_SDA, IOPORT_DIR_OUTPUT);

	ioport_set_pin_mode(ATSHA_SCK, IOPORT_MODE_WIREDANDPULL);
	ioport_set_pin_mode(ATSHA_SDA, IOPORT_MODE_WIREDANDPULL);
}
