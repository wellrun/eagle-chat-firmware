#include "usart.h"
#include <user_board.h>
#include "asf.h"

void usart_init(void) {

	ioport_set_pin_dir(DBG_RXD, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(DBG_TXD, IOPORT_DIR_OUTPUT);

	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
       .baudrate = 9600,
       .charlength = USART_CHSIZE_8BIT_gc,
       .paritytype = USART_PMODE_DISABLED_gc,
       .stopbits = false
    };
    
	sysclk_enable_module(SYSCLK_PORT_D, PR_USART0_bm);
	usart_init_rs232(&UART_DBG, &USART_SERIAL_OPTIONS);
	usart_tx_enable(&UART_DBG);
	usart_rx_enable(&UART_DBG);

}

void usart_write_char(uint8_t c) {
	usart_putchar(&UART_DBG, c);
}

void usart_write_string(uint8_t *buf) {
	const uint8_t *buffer = (uint8_t *)buf;
	while (*buffer != 0) {
		usart_write_char(*buffer);
		++buffer;
	}
}

void usart_newline() {
	usart_write_char('\n');
}

void usart_write_line(uint8_t *buf) {
	usart_write_string(buf);
	usart_newline();
}

uint8_t usart_read_char() {
	return usart_getchar(&UART_DBG);
}