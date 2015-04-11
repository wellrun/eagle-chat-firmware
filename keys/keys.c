#include "keys.h"
#include <common_nvm.h>
#include <string.h>

#define TABLE_BYTES_START   	PAGE_SIZE * PAGE_TABLE // the byte address the table starts at
#define STATUS_BYTES_START		PAGE_SIZE * PAGE_STATUS
#define PRIVATE_KEY_BYTES_START	PAGE_SIZE * PAGE_PRIVATE_KEY
#define PUBLIC_KEY_BYTES_START	PAGE_SIZE * PAGE_PUBLIC_KEY

// In-RAM storage objects:

static key_table_t key_table;

static key_setup_status_t status;

static uint8_t private_key[PAGE_SIZE];
static uint8_t public_key[PAGE_SIZE];

// Keys storage

uint8_t load_private_key() {
	return nvm_read(INT_EEPROM, PRIVATE_KEY_BYTES_START, private_key, PAGE_SIZE);
}

uint8_t * get_private_key(void) {
	return private_key;
}

uint8_t store_private_key(uint8_t key[PAGE_SIZE]) {
	return nvm_write(INT_EEPROM, PRIVATE_KEY_BYTES_START, key, PAGE_SIZE);
}

uint8_t load_public_key() {
	return nvm_read(INT_EEPROM, PUBLIC_KEY_BYTES_START, public_key, PAGE_SIZE);
}

uint8_t * get_public_key(void) {
	return public_key;
}

uint8_t store_public_key(uint8_t key[PAGE_SIZE]) {
	return nvm_write(INT_EEPROM, PUBLIC_KEY_BYTES_START, key, PAGE_SIZE);
}

// Setup status storage

uint8_t load_setup_status(void)
{
	memset((void*)&status, 0, sizeof(key_setup_status_t));  // zero out table (look at me being secure)
	if (nvm_init(INT_EEPROM) != STATUS_OK)
		return ERR_INVALID_ARG;

	return nvm_read(INT_EEPROM, STATUS_BYTES_START, (uint8_t*)&status, sizeof(key_setup_status_t));
}

const key_setup_status_t * get_setup_status(void)
{
	return (const key_setup_status_t*)&status;
}

uint8_t store_setup_status(void)
{
	return nvm_write(INT_EEPROM, STATUS_BYTES_START, (uint8_t*)&status, sizeof(key_setup_status_t));
}

// Flag storage

void set_status_flag(uint8_t mask)
{
	status.flags = KEYS_SET_BIT(status.flags, mask);
}

void unset_status_flag(uint8_t mask)
{
	status.flags = KEYS_CLEAR_BIT(status.flags, mask);
}

// SSK functions:


static bool ssk_find_free_slot(uint8_t * slot);


uint8_t ssk_load_table(void)
{
	memset((void*)&key_table, 0, sizeof(key_table_t));  // zero out table (look at me being secure)
	if (nvm_init(INT_EEPROM) != STATUS_OK)
		return ERR_INVALID_ARG;

	/* read the bytes into the table */
	return nvm_read(INT_EEPROM, TABLE_BYTES_START, (uint8_t*)&key_table, sizeof(key_table_t));
}

const key_table_t * ssk_get_table(void)
{
	return (const key_table_t*)&key_table;
}

/* Returns true if there is a table entry for the network id and stores it in slot */
bool ssk_has_key(uint8_t node_id, uint8_t * slot)
{
	for (uint8_t i = 0; i < MAX_KEY_SLOTS; ++i) {
		if (key_table.table[i].node_id == node_id) {
			*slot = key_table.table[i].key_page;
			return true;
		}
	}
	return false;
}

uint8_t ssk_store_key(uint8_t node_id, uint8_t key[PAGE_SIZE])
{
	uint8_t slot;

	if (ssk_has_key(node_id, &slot)) {
		if (slot >= MAX_KEY_SLOTS)
			return ERR_INVALID_ARG;
		return nvm_write(INT_EEPROM, (slot + PAGE_KEY_START) * PAGE_SIZE, key, PAGE_SIZE);
	} else {
		if (ssk_find_free_slot(&slot)) {
			/* Store the key first */
			if (STATUS_OK != nvm_write(INT_EEPROM, (slot + PAGE_KEY_START) * PAGE_SIZE, key, PAGE_SIZE))
				return ERR_INVALID_ARG;
			/* Now store the table entry */
			key_table.table[slot].node_id = node_id;
			key_table.table[slot].key_page = slot;
			return ssk_store_table();
		} else
			return ERR_INVALID_ARG; // For now, only fill table, do not overwrite
	}
}

uint8_t ssk_read_key(uint8_t slot, uint8_t dest[PAGE_SIZE])
{
	if (slot >= MAX_KEY_SLOTS)
		return ERR_INVALID_ARG;

	return nvm_read(INT_EEPROM, (slot + PAGE_KEY_START) * PAGE_SIZE, dest, PAGE_SIZE);
}


uint8_t ssk_store_table(void)
{
	return nvm_write(INT_EEPROM, TABLE_BYTES_START, (uint8_t*)&key_table, sizeof(key_table_t));
}

static bool ssk_find_free_slot(uint8_t * slot)
{
	for (uint8_t i = 0; i < MAX_KEY_SLOTS; ++i) {
		if (key_table.table[i].node_id == 0) {
			*slot = i;
			return true;
		}
	}
	return false;
}

uint8_t ssk_reset_table(void)
{
	uint8_t zeroes[PAGE_SIZE] = { 0 };

	return nvm_write(INT_EEPROM, TABLE_BYTES_START, zeroes, PAGE_SIZE);
}
