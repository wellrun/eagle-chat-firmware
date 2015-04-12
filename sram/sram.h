#ifndef SRAM_H_INCLUDED
#define SRAM_H_INCLUDED
#include "stdint.h"
void sram_init(void);
//void spi_init_pins(void);
void spi_init_module(void);
void write_mode(uint8_t);
void write_data(uint8_t[],uint16_t);
void read_data(uint16_t,uint8_t,uint8_t[]);
#endif
