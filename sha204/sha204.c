#include "sha204.h"
#include <string.h>

uint8_t lockZone(uint8_t zone);

uint8_t getDeviceRevision() {

	sha204p_set_device_id(SHA204_I2C_ADDRESS);

	uint8_t tx_buffer_command[DEVREV_COUNT];
	uint8_t rx_buffer[DEVREV_RSP_SIZE];
	uint8_t libreturn;

	struct sha204_dev_rev_parameters dev_rev;
	dev_rev.rx_buffer = rx_buffer;
	dev_rev.tx_buffer = tx_buffer_command;

	sha204p_set_device_id(200);
	memset(tx_buffer_command, 0, sizeof(tx_buffer_command));
	memset(rx_buffer, 0, sizeof(rx_buffer));

	libreturn = sha204m_dev_rev(&dev_rev);
	if (libreturn == SHA204_SUCCESS) {
		return rx_buffer[SHA204_BUFFER_POS_DATA + 3];
	} else {
		return -1;
	}
}

uint8_t lockZone(uint8_t zone) {

	sha204p_set_device_id(SHA204_I2C_ADDRESS);

	uint8_t tx_buffer_command[LOCK_COUNT];
	uint8_t rx_buffer[LOCK_RSP_SIZE];
	uint8_t libreturn;

	struct sha204_lock_parameters lock;
	lock.zone = zone;
	lock.tx_buffer = tx_buffer_command;
	lock.rx_buffer = rx_buffer;
	lock.summary = 0x0000;

	sha204p_set_device_id(200);
	memset(tx_buffer_command, 0, sizeof(tx_buffer_command));
	memset(rx_buffer, 0, sizeof(rx_buffer));

	libreturn = sha204m_lock(&lock);
	return libreturn;

}

uint8_t fullLock() {

	sha204p_set_device_id(SHA204_I2C_ADDRESS);

	uint8_t r1, r2;

	sha204p_wakeup();
	r1 = lockZone(0x80);
	sha204p_sleep();

	sha204h_delay_ms(5000);

	sha204p_wakeup();
	r2 = lockZone(0x81);

	sha204p_sleep();

	if ((r1 == 211) && (r2 = 211)) {
		return 1;
	} else {
		return 0;
	}
}

uint8_t getRandom32(uint8_t *destination) {

	sha204p_set_device_id(SHA204_I2C_ADDRESS);

	uint8_t tx_buffer_command[RANDOM_COUNT];
	uint8_t rx_buffer[RANDOM_RSP_SIZE];
	uint8_t libreturn;

	struct sha204_random_parameters random;
	random.mode = 0;
	random.tx_buffer = tx_buffer_command;
	random.rx_buffer = rx_buffer;

	sha204p_set_device_id(200);
	memset(tx_buffer_command, 0, sizeof(tx_buffer_command));
	memset(rx_buffer, 0, sizeof(rx_buffer));

	libreturn = sha204m_random(&random);

	if (libreturn == SHA204_SUCCESS) {
		memcpy(destination, &random.rx_buffer[SHA204_BUFFER_POS_DATA], 32);
		return 0;
	} else {
		memset(destination, 0, 32);
		return -1;
	}
}