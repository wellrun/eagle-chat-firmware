#ifndef SRAM_H_INCLUDED
#define SRAM_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

#define SRAM_MAX_ADDRESS 0x1FFF

typedef enum {
	SRAM_MODE_BYTE = 0,
	SRAM_MODE_PAGE = 0b10000000,
	SRAM_MODE_SEQUENTIAL = 0b01000000
} sram_mode_t;

typedef struct {
	uint8_t command;
	uint16_t address;
} sram_start_t;

void sram_init(void);

void sram_write_byte(uint16_t address, uint8_t byte);

void sram_write_packet(uint16_t address, const void * source, uint16_t len);

uint8_t sram_read_byte(uint16_t address);

uint8_t * sram_read_packet(uint16_t address, uint8_t * dest, uint16_t len);

bool sram_set_mode(sram_mode_t mode);

uint8_t sram_read_status(void);

#endif
