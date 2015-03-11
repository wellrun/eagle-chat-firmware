#ifndef KEYS_H__
#define KEYS_H__

#include "asf.h"
#include "crypto/avrnacl.h"

#define PAGE_SIZE           32
#define MAX_PAGES           64
#define MAX_BYTES           PAGE_SIZE * MAX_PAGES /* 2048 */
#define MAX_KEY_SLOTS       16

#define PAGE_STATUS         0
#define PAGE_PRIVATE_KEY    1
#define PAGE_PUBLIC_KEY     2
#define PAGE_TABLE          3 /* Page that holds the user key look-up table */
#define PAGE_KEY_START      4

typedef struct {
    uint8_t network_id;
    uint8_t key_page;
} key_table_entry_t;

typedef struct {
    key_table_entry_t table[MAX_KEY_SLOTS];
} key_table_t;


/*  Read the key lookup table from EEPROM and store it
    Returns STATUS_OK if all went well
*/
uint8_t keys_load_table(void);

uint8_t keys_store_table(void);

uint8_t keys_reset_table(void);

const key_table_t * keys_get_table(void);

/* Returns true if there is a table entry for the network id */
bool keys_has_key(uint8_t network_id, uint8_t * slot);

uint8_t keys_store_key(uint8_t network_id, uint8_t key[PAGE_SIZE]);

uint8_t keys_read_key(uint8_t slot, uint8_t dest[PAGE_SIZE]);

#endif
