#include "asf.h"
#include "cdc.h"
#include "keys.h"
//#include "eeprom.h"
#include <string.h>

int main(void)
{
	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	cdc_start();

	while (!cdc_opened());

	cdc_write_line("Beginning key store demo.");

	const key_table_t * table = keys_get_table();

	cdc_log_hex("Loading table: ", keys_load_table());
	cdc_log_hex_string("Table:\n", (void *)table, PAGE_SIZE);


	cdc_log_hex("Resetting table: ", keys_reset_table());
	keys_load_table();
	cdc_log_hex_string("Table:\n", (void *)table, PAGE_SIZE);

	uint8_t slot;
	uint8_t key[PAGE_SIZE];
	cdc_log_hex_string("Key to store:\n", (void *)key, PAGE_SIZE);
	cdc_log_hex("Looking for key for id=1. ", keys_has_key(1, &slot));

	cdc_log_hex("Storing key for id=1. ", keys_store_key(1, key));
	cdc_log_hex_string("Table:\n", (void *)table, PAGE_SIZE);

	cdc_log_hex("Looking for key for id=1. ", keys_has_key(1, &slot));
	cdc_log_hex("Key slot: ", slot);
	memset(key, 0xAA, PAGE_SIZE);
	cdc_log_hex_string("Blanked key:\n", (void *)key, PAGE_SIZE);
	keys_read_key(slot, key);
	cdc_log_hex_string("Read key:\n", (void *)key, PAGE_SIZE);

	cdc_write_line("Resetting key.");
	memset(key, 0xAA, PAGE_SIZE);
	cdc_log_hex("Storing key for id=1. ", keys_store_key(1, key));
	cdc_log_hex_string("Table:\n", (void *)table, PAGE_SIZE);
	memset(key, 0, PAGE_SIZE);
	keys_read_key(slot, key);
	cdc_log_hex_string("Read key:\n", (void *)key, PAGE_SIZE);

	cdc_newline(); cdc_newline();

	cdc_log_hex_string("Key to store:\n", (void *)key, PAGE_SIZE);
	cdc_log_hex("Looking for key for id=2. ", keys_has_key(2, &slot));

	cdc_log_hex("Storing key for id=2. ", keys_store_key(2, key));
	cdc_log_hex_string("Table:\n", (void *)table, PAGE_SIZE);

	cdc_log_hex("Looking for key for id=2. ", keys_has_key(2, &slot));
	cdc_log_hex("Key slot: ", slot);
	memset(key, 0xAA, PAGE_SIZE);
	cdc_log_hex_string("Blanked key:\n", (void *)key, PAGE_SIZE);
	keys_read_key(slot, key);
	cdc_log_hex_string("Read key:\n", (void *)key, PAGE_SIZE);

	cdc_write_line("Resetting key.");
	memset(key, 0xAA, PAGE_SIZE);
	cdc_log_hex("Storing key for id=2. ", keys_store_key(2, key));
	cdc_log_hex_string("Table:\n", (void *)table, PAGE_SIZE);
	memset(key, 0, PAGE_SIZE);
	keys_read_key(slot, key);
	cdc_log_hex_string("Read key:\n", (void *)key, PAGE_SIZE);

	while (1) {

	}

}
