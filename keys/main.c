#include "asf.h"
#include "cdc.h"
#include "keys.h"
#include <string.h>

int main(void)
{
	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	cdc_start();

	while (!cdc_opened());

	cdc_write_line("Beginning key store demo.");

	load_setup_status();

	const key_setup_status_t * status = get_setup_status();
	cdc_log_hex_string("Status:\n", status, sizeof(key_setup_status_t));
	cdc_newline();

	const key_table_t * table = ssk_get_table();
	cdc_log_hex("Loading table: ", ssk_load_table());
	cdc_log_hex_string("Table:\n", table, PAGE_SIZE);
	cdc_newline();

	if (!KEYS_BIT_SET(status->flags, FLAGS_CONFIGURED)) {
		cdc_write_line("Device is not initialized. Resetting table");

		cdc_log_hex("Resetting table: ", ssk_reset_table());
		ssk_load_table();
		cdc_log_hex_string("Table:\n", table, PAGE_SIZE);
		set_status_flag(FLAGS_CONFIGURED);
		store_setup_status();
	}

	cdc_newline(); cdc_newline();

	uint8_t slot;
	bool has_key;
	uint8_t key[PAGE_SIZE];
	for (uint8_t user = 1; user <= MAX_KEY_SLOTS; ++user) {
		slot = 0;
		has_key = false;
		memset(key, 0xA, PAGE_SIZE);
		cdc_log_int("Testing user=", user);
		has_key = ssk_has_key(user, &slot);
		cdc_log_string("Has key should be false: ", has_key ? "true" : "false");
		cdc_log_string("Storing key: ", ssk_store_key(user, key) == STATUS_OK ? "success" : "failure");
		has_key = ssk_has_key(user, &slot);
		cdc_log_string("Has key should be true: ", has_key ? "true" : "false");
		if (has_key) {
			memset(key, 0x0, PAGE_SIZE);
			ssk_read_key(slot, key);
			cdc_log_hex_string("Read key:\n", key, PAGE_SIZE);
			cdc_write_line("Storing new key in old slot.");
			memset(key, 0xE, PAGE_SIZE);
			cdc_log_string("Storing key: ", ssk_store_key(user, key) == STATUS_OK ? "success" : "failure");
			has_key = ssk_has_key(user, &slot);
			cdc_log_string("Has key should be true: ", has_key ? "true" : "false");
			if (has_key) {
				ssk_read_key(slot, key);
				cdc_log_hex_string("Read key:\n", key, PAGE_SIZE);
			}
		}
		cdc_newline();
	}

	cdc_newline();

	ssk_load_table();
	cdc_log_hex_string("Table:\n", table, PAGE_SIZE);

	uint8_t pkey[PAGE_SIZE];
	memset(pkey, 0xA, PAGE_SIZE);

	load_private_key();
	cdc_log_hex_string("Private key: ", get_private_key(), PAGE_SIZE);
	cdc_log_string("Storing key: ", store_private_key(pkey) == STATUS_OK ? "success" : "failure");

	load_private_key();
	cdc_log_hex_string("Private key: ", get_private_key(), PAGE_SIZE);

	load_public_key();
	cdc_log_hex_string("Public key: ", get_public_key(), PAGE_SIZE);
	cdc_log_string("Storing key: ", store_public_key(pkey) == STATUS_OK ? "success" : "failure");

	load_public_key();
	cdc_log_hex_string("Public key: ", get_public_key(), PAGE_SIZE);

	while (1) {

	}

}
