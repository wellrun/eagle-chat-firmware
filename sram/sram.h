#ifndef SRAM_H_INCLUDED
#define SRAM_H_INCLUDED

void sram_init(void);
//void spi_init_pins(void);
void spi_init_module(void);
void write_mode(void);
void write_data(void);
void read_data(void);
#endif
