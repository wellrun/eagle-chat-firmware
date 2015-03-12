#include "keys.h"
#include <common_nvm.h>
#include <string.h>

static key_table_t key_table;

#define TABLE_BYTES_START   PAGE_SIZE * PAGE_TABLE // the byte address the table starts at
#define STATUS_BYTES_START  PAGE_SIZE * PAGE_STATUS

static key_setup_status_t status;

uint8_t keys_load_status(void)
{
	memset((void*)&status, 0, sizeof(key_setup_status_t));  // zero out table (look at me being secure)
	if (nvm_init(INT_EEPROM) != STATUS_OK)
		return ERR_INVALID_ARG;

	return nvm_read(INT_EEPROM, STATUS_BYTES_START, (uint8_t*)&status, sizeof(key_setup_status_t));
}

const key_setup_status_t * keys_get_status(void)
{
	return (const key_setup_status_t*)&status;
}

uint8_t keys_store_status(void)
{
	return nvm_write(INT_EEPROM, STATUS_BYTES_START, (uint8_t*)&status, sizeof(key_setup_status_t));
}

void keys_set_flag(uint8_t mask)
{
	status.flags = KEYS_SET_BIT(status.flags, mask);
}

void keys_unset_flag(uint8_t mask)
{
	status.flags = KEYS_CLEAR_BIT(status.flags, mask);
}


static bool keys_find_free_slot(uint8_t * slot);


uint8_t keys_load_table(void)
{
	memset((void*)&key_table, 0, sizeof(key_table_t));  // zero out table (look at me being secure)
	if (nvm_init(INT_EEPROM) != STATUS_OK)
		return ERR_INVALID_ARG;

	/* read the bytes into the table */
	return nvm_read(INT_EEPROM, TABLE_BYTES_START, (uint8_t*)&key_table, sizeof(key_table_t));
}

const key_table_t * keys_get_table(void)
{
	return (const key_table_t*)&key_table;
}

/* Returns true if there is a table entry for the network id and stores it in slot */
bool keys_has_key(uint8_t network_id, uint8_t * slot)
{
	for (uint8_t i = 0; i < MAX_KEY_SLOTS; ++i) {
		if (key_table.table[i].network_id == network_id) {
			*slot = key_table.table[i].key_page;
			return true;
		}
	}
	return false;
}

uint8_t keys_store_key(uint8_t network_id, uint8_t key[PAGE_SIZE])
{
	uint8_t slot;

	if (keys_has_key(network_id, &slot)) {
		if (slot >= MAX_KEY_SLOTS)
			return ERR_INVALID_ARG;
		return nvm_write(INT_EEPROM, (slot + PAGE_KEY_START) * PAGE_SIZE, key, PAGE_SIZE);
	} else {
		if (keys_find_free_slot(&slot)) {
			/* Store the key first */
			if (STATUS_OK != nvm_write(INT_EEPROM, (slot + PAGE_KEY_START) * PAGE_SIZE, key, PAGE_SIZE))
				return ERR_INVALID_ARG;
			/* Now store the table entry */
			key_table.table[slot].network_id = network_id;
			key_table.table[slot].key_page = slot;
			return keys_store_table();
		} else
			return ERR_INVALID_ARG; // For now, only fill table, do not overwrite
	}
}

uint8_t keys_read_key(uint8_t slot, uint8_t dest[PAGE_SIZE])
{
	if (slot >= MAX_KEY_SLOTS)
		return ERR_INVALID_ARG;

	return nvm_read(INT_EEPROM, (slot + PAGE_KEY_START) * PAGE_SIZE, dest, PAGE_SIZE);
}


uint8_t keys_store_table(void)
{
	return nvm_write(INT_EEPROM, TABLE_BYTES_START, (uint8_t*)&key_table, sizeof(key_table_t));
}

static bool keys_find_free_slot(uint8_t * slot)
{
	for (uint8_t i = 0; i < MAX_KEY_SLOTS; ++i) {
		if (key_table.table[i].network_id == 0) {
			*slot = i;
			return true;
		}
	}
	return false;
}

uint8_t keys_reset_table(void)
{
	uint8_t zeroes[PAGE_SIZE] = { 0 };

	return nvm_write(INT_EEPROM, TABLE_BYTES_START, zeroes, PAGE_SIZE);
}
