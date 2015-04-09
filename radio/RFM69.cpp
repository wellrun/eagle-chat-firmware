// **********************************************************************************
// Driver definition for HopeRF RFM69W/RFM69HW/RFM69CW/RFM69HCW, Semtech SX1231/1231H
// **********************************************************************************
// Copyright Felix Rusu (2014), felix@lowpowerlab.com
// http://lowpowerlab.com/
// **********************************************************************************
// License
// **********************************************************************************
// This program is free software; you can redistribute it
// and/or modify it under the terms of the GNU General
// Public License as published by the Free Software
// Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General
// Public License along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//
// Licence can be viewed at
// http://www.gnu.org/licenses/gpl-3.0.txt
//
// Please maintain this license information along with authorship
// and copyright notices in any redistribution of this code
// **********************************************************************************
#include "RFM69.h"
#include "RFM69registers.h"

#include "user_board.h"
#include "util/atomic.h"

#include <crc.h>

#include <string.h>

struct spi_device spi_device_conf = {
	.id = IOPORT_CREATE_PIN(PORTE, 4)
};

void SPIBegin() {
	ioport_configure_port_pin(&PORTE, PIN4_bm, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);  // Own CS being used as Module CS
	ioport_configure_port_pin(&PORTE, PIN5_bm, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);  // MOSI
	ioport_configure_port_pin(&PORTE, PIN6_bm, IOPORT_DIR_INPUT);                      // MISO
	ioport_configure_port_pin(&PORTE, PIN7_bm, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);  // SCL


	spi_master_init(&SPIE);
	spi_master_setup_device(&SPIE, &spi_device_conf, SPI_MODE_0, 4500000, 0);
	spi_enable(&SPIE);
}

uint8_t SPITransfer(const uint8_t val) {
	uint8_t ret;
	if (val == 0) {
		spi_write_single(&SPIE,0); //Dummy write
		while (!spi_is_rx_full(&SPIE)) {
		}
		spi_read_single(&SPIE, &ret);
		return ret;
	} else {
		spi_write_single(&SPIE, val);
		while (!spi_is_rx_full(&SPIE)) {
		}
		return 0;
	}
}

int readInterruptPin() {
	return 0;
}

uint32_t millis() {
	/*static uint32_t currentTime = 0;
	currentTime += rtc_get_time();
	rtc_set_time(0);
	return currentTime;
	*/
	return rtc_get_time();
}

inline void radio_set_interrupt_pin() {
	RF_DIO_PORT.INT0MASK = ioport_pin_to_mask(RF_DIO2_pin);
}

inline void radio_clear_interrupt_pin() {
	RF_DIO_PORT.INT0MASK = 0;
}

inline void radio_enable_interrupt() {
	RF_DIO_PORT.INTCTRL =  (RF_DIO_PORT.INTCTRL & ~PORT_INT0LVL_gm) | PORT_INT0LVL_HI_gc;
	//RF_DIO_PORT.INT0MASK = 1;
	//sei();
}

inline void radio_disable_interrupt() {
	RF_DIO_PORT.INTCTRL =  (RF_DIO_PORT.INTCTRL & ~PORT_INT0LVL_gm) | PORT_INT0LVL_OFF_gc;
	//RF_DIO_PORT.INT0MASK = 0;
	//cli();
}

fifo_t  		 RFM69::RXFIFO;
volatile uint8_t RFM69::DATA[RF69_MAX_DATA_LEN];
volatile uint8_t RFM69::_mode;        // current transceiver state
volatile uint8_t RFM69::DATALEN;
volatile uint8_t RFM69::SENDERID;
volatile uint8_t RFM69::TARGETID;     // should match _address
volatile uint8_t RFM69::PAYLOADLEN;
volatile uint8_t RFM69::ACK_REQUESTED;
volatile uint8_t RFM69::ACK_RECEIVED; // should be polled immediately after sending a packet with ACK request
volatile int16_t RFM69::RSSI;          // most accurate RSSI during reception (closest to the reception)
RFM69* RFM69::selfPointer;

volatile bool ACK_RECEIVED_CLEARED = true;

bool RFM69::initialize(uint8_t freqBand, uint8_t nodeID, uint8_t networkID)
{
	const uint8_t CONFIG[][2] =
	{
		/* 0x01 */ { REG_OPMODE, RF_OPMODE_SEQUENCER_ON | RF_OPMODE_LISTEN_OFF | RF_OPMODE_STANDBY },
		/* 0x02 */ { REG_DATAMODUL, RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_FSK | RF_DATAMODUL_MODULATIONSHAPING_00 }, // no shaping
		/* 0x03 */ { REG_BITRATEMSB, RF_BITRATEMSB_55555}, // default: 4.8 KBPS
		/* 0x04 */ { REG_BITRATELSB, RF_BITRATELSB_55555},
		/* 0x05 */ { REG_FDEVMSB, RF_FDEVMSB_50000}, // default: 5KHz, (FDEV + BitRate / 2 <= 500KHz)
		/* 0x06 */ { REG_FDEVLSB, RF_FDEVLSB_50000},

		/* 0x07 */ { REG_FRFMSB, (uint8_t) (freqBand==RF69_315MHZ ? RF_FRFMSB_315 : (freqBand==RF69_433MHZ ? RF_FRFMSB_433 : (freqBand==RF69_868MHZ ? RF_FRFMSB_868 : RF_FRFMSB_915))) },
		/* 0x08 */ { REG_FRFMID, (uint8_t) (freqBand==RF69_315MHZ ? RF_FRFMID_315 : (freqBand==RF69_433MHZ ? RF_FRFMID_433 : (freqBand==RF69_868MHZ ? RF_FRFMID_868 : RF_FRFMID_915))) },
		/* 0x09 */ { REG_FRFLSB, (uint8_t) (freqBand==RF69_315MHZ ? RF_FRFLSB_315 : (freqBand==RF69_433MHZ ? RF_FRFLSB_433 : (freqBand==RF69_868MHZ ? RF_FRFLSB_868 : RF_FRFLSB_915))) },

		// looks like PA1 and PA2 are not implemented on RFM69W, hence the max output power is 13dBm
		// +17dBm and +20dBm are possible on RFM69HW
		// +13dBm formula: Pout = -18 + OutputPower (with PA0 or PA1**)
		// +17dBm formula: Pout = -14 + OutputPower (with PA1 and PA2)**
		// +20dBm formula: Pout = -11 + OutputPower (with PA1 and PA2)** and high power PA settings (section 3.3.7 in datasheet)
		///* 0x11 */ { REG_PALEVEL, RF_PALEVEL_PA0_ON | RF_PALEVEL_PA1_OFF | RF_PALEVEL_PA2_OFF | RF_PALEVEL_OUTPUTPOWER_11111},
		///* 0x13 */ { REG_OCP, RF_OCP_ON | RF_OCP_TRIM_95 }, // over current protection (default is 95mA)

		// RXBW defaults are { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24 | RF_RXBW_EXP_5} (RxBw: 10.4KHz)
		/* 0x19 */ { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16 | RF_RXBW_EXP_2 }, // (BitRate < 2 * RxBw)
		//for BR-19200: /* 0x19 */ { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24 | RF_RXBW_EXP_3 },
		/* 0x25 */ { REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_00 }, // DIO0 is the only IRQ we're using
		/* 0x26 */ { REG_DIOMAPPING2, RF_DIOMAPPING2_CLKOUT_OFF }, // DIO5 ClkOut disable for power saving
		/* 0x28 */ { REG_IRQFLAGS2, RF_IRQFLAGS2_FIFOOVERRUN }, // writing to this bit ensures that the FIFO & status flags are reset
		/* 0x29 */ { REG_RSSITHRESH, 220 }, // must be set to dBm = (-Sensitivity / 2), default is 0xE4 = 228 so -114dBm
		///* 0x2D */ { REG_PREAMBLELSB, RF_PREAMBLESIZE_LSB_VALUE } // default 3 preamble bytes 0xAAAAAA
		/* 0x2E */ { REG_SYNCCONFIG, RF_SYNC_ON | RF_SYNC_FIFOFILL_AUTO | RF_SYNC_SIZE_4 | RF_SYNC_TOL_0 },
		/* 0x2F */ { REG_SYNCVALUE1, 0x2D },      // attempt to make this compatible with sync1 byte of RFM12B lib
		/* 0x30 */ { REG_SYNCVALUE2, networkID }, // NETWORK ID
		/* 0x31 */ { REG_SYNCVALUE3, 0xAE }, 	  // Extra sync byte
		/* 0x32 */ { REG_SYNCVALUE4, 0x75 }, 	  // Extra sync byte
		/* 0x37 */ { REG_PACKETCONFIG1, RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_WHITENING | RF_PACKET1_CRC_OFF | RF_PACKET1_CRCAUTOCLEAR_OFF | RF_PACKET1_ADRSFILTERING_OFF },
		/* 0x38 */ { REG_PAYLOADLENGTH, 255 }, // in variable length mode: the max frame size, not used in TX
		///* 0x39 */ { REG_NODEADRS, nodeID }, // turned off because we're not using address filtering
		/* 0x3C */ { REG_FIFOTHRESH, RF_FIFOTHRESH_TXSTART_FIFONOTEMPTY | RF_FIFOTHRESH_VALUE }, // TX on FIFO not empty
		/* 0x3D */ { REG_PACKETCONFIG2, RF_PACKET2_RXRESTARTDELAY_2BITS | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF }, // RXRESTARTDELAY must match transmitter PA ramp-down time (bitrate dependent)
		//for BR-19200: /* 0x3D */ { REG_PACKETCONFIG2, RF_PACKET2_RXRESTARTDELAY_NONE | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF }, // RXRESTARTDELAY must match transmitter PA ramp-down time (bitrate dependent)
		/* 0x6F */ { REG_TESTDAGC, RF_DAGC_IMPROVED_LOWBETA0 }, // run DAGC continuously in RX mode for Fading Margin Improvement, recommended default for AfcLowBetaOn=0
		{255, 0}
	};
	SPIBegin();

	writeReg(REG_SYNCVALUE1, 0xAA);
	while (readReg(REG_SYNCVALUE1) != 0xAA);
	writeReg(REG_SYNCVALUE1, 0x55);
	while (readReg(REG_SYNCVALUE1) != 0x55);

	for (uint8_t i = 0; CONFIG[i][0] != 255; i++)
		writeReg(CONFIG[i][0], CONFIG[i][1]);

	// Encryption is persistent between resets and can trip you up during debugging.
	// Disable it during initialization so we always start from a known state.
	encrypt(0);

	setHighPower(_isRFM69HW); // called regardless if it's a RFM69W or RFM69HW
	setMode(RF69_MODE_STANDBY);
	while ((readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) == 0x00);

	ioport_set_pin_dir(RF_DIO2_pin, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(RF_DIO0_pin, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(RF_DIO1_pin, IOPORT_DIR_INPUT);
	ioport_set_pin_sense_mode(RF_DIO2_pin, IOPORT_SENSE_RISING);
	radio_set_interrupt_pin();
	radio_enable_interrupt();
	//RF_DIO_PORT.INT0MASK = ;
	sei();

	fifo_init(&RXFIFO);

	selfPointer = this;
	_address = nodeID;
	return true;
}

// return the frequency (in Hz)
uint32_t RFM69::getFrequency()
{
	return RF69_FSTEP * (((uint32_t) readReg(REG_FRFMSB) << 16) + ((uint16_t) readReg(REG_FRFMID) << 8) + readReg(REG_FRFLSB));
}

// set the frequency (in Hz)
void RFM69::setFrequency(uint32_t freqHz)
{
	uint8_t oldMode = _mode;
	if (oldMode == RF69_MODE_TX) {
		setMode(RF69_MODE_RX);
	}
	freqHz /= RF69_FSTEP; // divide down by FSTEP to get FRF
	writeReg(REG_FRFMSB, freqHz >> 16);
	writeReg(REG_FRFMID, freqHz >> 8);
	writeReg(REG_FRFLSB, freqHz);
	if (oldMode == RF69_MODE_RX) {
		setMode(RF69_MODE_SYNTH);
	}
	setMode(oldMode);
}

void RFM69::setMode(uint8_t newMode)
{
	if (newMode == _mode)
		return;

	switch (newMode) {
		case RF69_MODE_TX:
			writeReg(REG_OPMODE, (readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_TRANSMITTER);
			if (_isRFM69HW) setHighPowerRegs(true);
			break;
		case RF69_MODE_RX:
			writeReg(REG_OPMODE, (readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_RECEIVER);
			if (_isRFM69HW) setHighPowerRegs(false);
			break;
		case RF69_MODE_SYNTH:
			writeReg(REG_OPMODE, (readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_SYNTHESIZER);
			break;
		case RF69_MODE_STANDBY:
			writeReg(REG_OPMODE, (readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_STANDBY);
			break;
		case RF69_MODE_SLEEP:
			writeReg(REG_OPMODE, (readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_SLEEP);
			break;
		default:
			return;
	}

	// we are using packet mode, so this check is not really needed
	// but waiting for mode ready is necessary when going from sleep because the FIFO may not be immediately available from previous mode
	while (_mode == RF69_MODE_SLEEP && (readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) == 0x00); // wait for ModeReady

	_mode = newMode;
}

void RFM69::sleep() {
	setMode(RF69_MODE_SLEEP);
}

void RFM69::setAddress(uint8_t addr)
{
	_address = addr;
	writeReg(REG_NODEADRS, _address);
}

void RFM69::setNetwork(uint8_t networkID)
{
	writeReg(REG_SYNCVALUE2, networkID);
}

// set output power: 0 = min, 31 = max
// this results in a "weaker" transmitted signal, and directly results in a lower RSSI at the receiver
void RFM69::setPowerLevel(uint8_t powerLevel)
{
	_powerLevel = powerLevel;
	writeReg(REG_PALEVEL, (readReg(REG_PALEVEL) & 0xE0) | (_powerLevel > 31 ? 31 : _powerLevel));
}

bool RFM69::canSend()
{
	if (_mode == RF69_MODE_RX && PAYLOADLEN == 0 && readRSSI() < CSMA_LIMIT) // if signal stronger than -100dBm is detected assume channel activity
	{
		setMode(RF69_MODE_STANDBY);
		return true;
	}
	return false;
}

void RFM69::send(uint8_t toAddress, const void* buffer, uint8_t bufferSize, bool requestACK)
{
	writeReg(REG_PACKETCONFIG2, (readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
	uint32_t now = millis();
	while (!canSend() && millis() - now < RF69_CSMA_LIMIT_MS) receiveDone();
	sendFrame(toAddress, buffer, bufferSize, requestACK, false);
}

// to increase the chance of getting a packet across, call this function instead of send
// and it handles all the ACK requesting/retrying for you :)
// The only twist is that you have to manually listen to ACK requests on the other side and send back the ACKs
// The reason for the semi-automaton is that the lib is interrupt driven and
// requires user action to read the received data and decide what to do with it
// replies usually take only 5..8ms at 50kbps@915MHz
bool RFM69::sendWithRetry(uint8_t toAddress, const void* buffer, uint8_t bufferSize, uint8_t retries, uint16_t retryWaitTime) {
	uint32_t sentTime;
	for (uint8_t i = 0; i <= retries; i++)
	{
		////cdc_log_int("Attempting to send. ", rtc_get_time());
		send(toAddress, buffer, bufferSize, true);
		sentTime = millis();
		////cdc_write_line("Poll for ACK.");
		radio_enable_interrupt();
		while (millis() - sentTime < retryWaitTime)
		{
			if (ACKReceived(toAddress))
			{
				//Serial.print(" ~ms:"); Serial.print(millis() - sentTime);
				//cdc_log_int("Ack response time(ms): ", millis() - sentTime);
				return true;
			}
		}
		//cdc_log_int("Failed attempt #", i + 1);
		//Serial.print(" RETRY#"); Serial.println(i + 1);
	}
	return false;
}

// should be polled immediately after sending a packet with ACK request
bool RFM69::ACKReceived(uint8_t fromNodeID) {
	if (receiveDone()) {
		bool ret = (SENDERID == fromNodeID || fromNodeID == RF69_BROADCAST_ADDR) && ACK_RECEIVED;
		if (ret)
			ACK_RECEIVED_CLEARED = true; // clear ack received because we're return true
		return ret;
	}
	if (!ACK_RECEIVED_CLEARED) {
		ACK_RECEIVED_CLEARED = true;
		receiveBegin();
		return true;
	}
	receiveBegin();
	return false;
}

// check whether an ACK was requested in the last received packet (non-broadcasted packet)
bool RFM69::ACKRequested() {
	return ACK_REQUESTED && (TARGETID != RF69_BROADCAST_ADDR);
}

// should be called immediately after reception in case sender wants ACK
void RFM69::sendACK(const void* buffer, uint8_t bufferSize) {
	uint8_t sender = SENDERID;
	int16_t _RSSI = RSSI; // save payload received RSSI value
	writeReg(REG_PACKETCONFIG2, (readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
	uint32_t now = millis();
	while (!canSend() && millis() - now < RF69_CSMA_LIMIT_MS) receiveDone();
	sendFrame(sender, buffer, bufferSize, false, true);
	RSSI = _RSSI; // restore payload RSSI
}

void RFM69::simpleSendACK(uint8_t address) {
	uint8_t sender = address;
	int16_t _RSSI = RSSI; // save payload received RSSI value
	writeReg(REG_PACKETCONFIG2, (readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
	uint32_t now = millis();
	while (!canSend() && millis() - now < RF69_CSMA_LIMIT_MS) receiveDone();
	sendFrame(sender, "", 0, false, true);
	RSSI = _RSSI; // restore payload RSSI
}

static inline bool radio_fifoNotEmpty(void) {
	return ioport_get_pin_level(RF_DIO2_pin);
}

static inline bool radio_fifoFull(void) {
	return ioport_get_pin_level(RF_DIO3_pin);
}

static inline bool radio_CrcOk(void) {
	return ioport_get_pin_level(RF_DIO0_pin);
}



void RFM69::sendFrame(uint8_t toAddress, const void* buffer, uint8_t bufferSize, bool requestACK, bool sendACK)
{
	setMode(RF69_MODE_STANDBY); // turn off receiver to prevent reception while filling fifo
	while ((readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) == 0x00); // wait for ModeReady
	//writeReg(REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_00); // DIO0 is "Packet Sent"
	if (bufferSize > RF69_MAX_DATA_LEN) bufferSize = RF69_MAX_DATA_LEN;

	// control byte
	uint8_t CTLbyte = 0x00;
	if (sendACK)
		CTLbyte = 0x80;
	else if (requestACK)
		CTLbyte = 0x40;

	// initialize checksum calculation
	crc_io_checksum_byte_start(CRC_16BIT);

	uint8_t length = bufferSize + 3 + 2;

	// write to FIFO
	select();
		SPITransfer(REG_FIFO | 0x80);
		SPITransfer(length); // 3 bytes for the following frame header; 2 bytes for manual CRC at the end
		crc_io_checksum_byte_add(length);
		SPITransfer(toAddress);
		crc_io_checksum_byte_add(toAddress);
		SPITransfer(_address);
		crc_io_checksum_byte_add(_address);
		SPITransfer(CTLbyte);
		crc_io_checksum_byte_add(CTLbyte);

		// fifo has 4 bytes, 66 - 4 = 62 remaining

		//uint8_t prefill = bufferSize > 61 ? 61 : bufferSize;
		uint8_t i = 0;
		//for (; i < prefill; i++)
		//	SPITransfer(((uint8_t*) buffer)[i]);

		/* Prefill the FIFO until we're out of message or room in the buffer */
		while (!radio_fifoFull() && i < bufferSize) {
			SPITransfer(((uint8_t*) buffer)[i]);
			crc_io_checksum_byte_add(((uint8_t*) buffer)[i]);
			++i;
		}
	unselect();

	// no need to wait for transmit mode to be ready since its handled by the radio
	setMode(RF69_MODE_TX);

	// Radio is now transmitting our data and fifo is being emptied

	select();
		SPITransfer(REG_FIFO | 0x80);
		if (i < bufferSize) { // if we still have bytes to send
			while(i < bufferSize) {
				//while (!radio_fifoNotEmpty()); // wait till fifo has room
				while (!radio_fifoFull() && i < bufferSize) { // avoid overfilling fifo
					SPITransfer(((uint8_t*) buffer)[i]); // send byte
					crc_io_checksum_byte_add(((uint8_t*) buffer)[i]);
					++i;
				}
			}
		}
		uint16_t result = (uint16_t)crc_io_checksum_byte_stop();

		// Write the CRC bits
		while (radio_fifoFull());
		SPITransfer(((uint8_t *)&result)[0]);
		while (radio_fifoFull());
		SPITransfer(((uint8_t *)&result)[1]);

	unselect();

	//cdc_log_int("sendFrame CRC result: ", result);

	uint32_t txStart = millis();
	while (ioport_get_pin_level(_interruptPin) == 0 && millis() - txStart < RF69_TX_LIMIT_MS); // wait for DIO0 to turn HIGH signalling transmission finish
	//while (readReg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PACKETSENT == 0x00); // wait for ModeReady
	setMode(RF69_MODE_STANDBY);
}

/*
	Receive interrupt handler. Is triggered by pin DIO2, which goes high on radio's FifoNotEmpty
*/
void RFM69::interruptHandler() {
	// Check that we are in receive mode and the FifoNotEmpty interrupt has fired
	if (_mode == RF69_MODE_RX && (readReg(REG_IRQFLAGS2) & RF_IRQFLAGS2_FIFONOTEMPTY))
	{
		uint32_t now = millis();
		uint32_t then;

		radio_disable_interrupt();
		radio_clear_interrupt_pin();

		// Start checksum calculation
		crc_io_checksum_byte_start(CRC_16BIT);

		select();
		SPITransfer(REG_FIFO & 0x7F);
		PAYLOADLEN = SPITransfer(0);
		crc_io_checksum_byte_add(PAYLOADLEN);

		if (PAYLOADLEN < 5) { // BAIL!!!!
			unselect();
			receiveBegin();
			return;
		}

		while (!radio_fifoNotEmpty());

		TARGETID = SPITransfer(0);
		crc_io_checksum_byte_add(TARGETID);
		DATA[0]  = TARGETID; // Store TARGETID


		if(!(_promiscuousMode || TARGETID == _address || TARGETID == RF69_BROADCAST_ADDR) // match this node's address, or broadcast address or anything in promiscuous mode
			 || PAYLOADLEN < 5) // address situation could receive packets that are malformed and don't fit this libraries extra fields
		{
			unselect();
			setMode(RF69_MODE_STANDBY); // Need to change modes to clear the FIFO. TODO: find quicker way to do that
			receiveBegin();
			return;
		}

		DATALEN = PAYLOADLEN - 3 - 2;

		while (!radio_fifoNotEmpty());
		SENDERID = SPITransfer(0);
		crc_io_checksum_byte_add(SENDERID);
		DATA[1] = SENDERID; // Store SENDERID

		while (!radio_fifoNotEmpty());
		uint8_t CTLbyte = SPITransfer(0);
		crc_io_checksum_byte_add(CTLbyte);
		DATA[2] = CTLbyte; // Store CTLbyte

		DATA[3] = DATALEN; // Store DATALEN

		ACK_RECEIVED = CTLbyte & 0x80; // extract ACK-received flag
		ACK_REQUESTED = CTLbyte & 0x40; // extract ACK-requested flag

		uint8_t i = 0;
		uint8_t crc_okay = false;

		while (i < DATALEN) {
			while (!radio_fifoNotEmpty());
			DATA[i+4] = SPITransfer(0); //  i+4 because 0,1,2,3 in DATA are prefix bytes + DATALEN
			crc_io_checksum_byte_add(DATA[i+4]);
			++i;
		}

		uint8_t crc_bytes[2];
		while (!radio_fifoNotEmpty());
		crc_bytes[0] = SPITransfer(0);
		while (!radio_fifoNotEmpty());
		crc_bytes[1] = SPITransfer(0);

		unselect();

		setMode(RF69_MODE_STANDBY); // Packet is finished receiving, safe to go to standby

		uint16_t our_crc = (uint16_t) crc_io_checksum_byte_stop();
		uint16_t their_crc = *((uint16_t *)crc_bytes);

		crc_okay = (our_crc == their_crc);

		if (!crc_okay) { // Bail if crc's not okay
			receiveBegin();
			return;
		}

		if (ACK_RECEIVED) { // Don't fill fifo is this was just an ACK
			//setMode(RF69_MODE_RX);
			ACK_RECEIVED_CLEARED = false;
			radio_set_interrupt_pin();
			then = millis() - now;
			//cdc_log_int("Total time: ", then);
			return;
		}

		fifo_write(&RXFIFO, (uint8_t *)DATA, DATALEN+4);

		then = millis() - now;
		//cdc_log_int("Total time: ", then);

		//setMode(RF69_MODE_RX);
		radio_set_interrupt_pin();
	}
	RSSI = readRSSI();
	//digitalWrite(4, 0);
}

//void RFM69::isr0() { selfPointer->interruptHandler(); }

ISR(PORTF_INT0_vect) { RFM69::selfPointer->interruptHandler(); }

void RFM69::receiveBegin() {
	DATALEN = 0;
	SENDERID = 0;
	TARGETID = 0;
	PAYLOADLEN = 0;
	ACK_REQUESTED = 0;
	ACK_RECEIVED = 0;
	RSSI = 0;
	if (readReg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PAYLOADREADY)
		writeReg(REG_PACKETCONFIG2, (readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
	writeReg(REG_DIOMAPPING1, RF_DIOMAPPING1_DIO2_00 | RF_DIOMAPPING1_DIO0_00); // set DIO2 to "FifoNotEmpty" in receive mode, DIO0 is payloadready
	setMode(RF69_MODE_RX);
	radio_set_interrupt_pin();
}

bool RFM69::receiveDone() {
	////cdc_write_line("receiveDone()");
	//ATOMIC_BLOCK(ATOMIC_FORCEON){
		radio_disable_interrupt();//noInterrupts(); // re-enabled in unselect() via setMode() or via receiveBegin()
		if (_mode == RF69_MODE_RX && PAYLOADLEN > 0)
		{
			////cdc_write_line("rcd = true");
			setMode(RF69_MODE_STANDBY); // enables interrupts
			return true;
		}
		else if (_mode == RF69_MODE_RX) // already in RX no payload yet
		{
			radio_enable_interrupt();//interrupts(); // explicitly re-enable interrupts
			return false;
		}
		receiveBegin();
		return false;
	//}
}

// To enable encryption: radio.encrypt("ABCDEFGHIJKLMNOP");
// To disable encryption: radio.encrypt(null) or radio.encrypt(0)
// KEY HAS TO BE 16 bytes !!!
void RFM69::encrypt(const char* key) {
	setMode(RF69_MODE_STANDBY);
	if (key != 0)
	{
		select();
		SPITransfer(REG_AESKEY1 | 0x80);
		for (uint8_t i = 0; i < 16; i++)
			SPITransfer(key[i]);
		unselect();
	}
	writeReg(REG_PACKETCONFIG2, (readReg(REG_PACKETCONFIG2) & 0xFE) | (key ? 1 : 0));
}

int16_t RFM69::readRSSI(bool forceTrigger) {
	int16_t rssi = 0;
	if (forceTrigger)
	{
		// RSSI trigger not needed if DAGC is in continuous mode
		writeReg(REG_RSSICONFIG, RF_RSSI_START);
		while ((readReg(REG_RSSICONFIG) & RF_RSSI_DONE) == 0x00); // wait for RSSI_Ready
	}
	rssi = -(readReg(REG_RSSIVALUE)/2);
	//rssi >>= 1;
	//rssi = -rssi;
	return rssi;
}

uint8_t RFM69::readReg(uint8_t addr)
{
	//_delay_ms(SPIDELAY);
	select();
	SPITransfer(addr & 0x7F);
	uint8_t regval = SPITransfer(0);
	unselect();
	return regval;
}

void RFM69::writeReg(uint8_t addr, uint8_t value)
{
	//_delay_ms(SPIDELAY);
	select();
	SPITransfer(addr | 0x80);
	SPITransfer(value);
	unselect();
}

// select the transceiver
void RFM69::select() {
	radio_disable_interrupt();//noInterrupts();

	spi_select_device(&SPIE, &spi_device_conf);
}

// UNselect the transceiver chip
void RFM69::unselect() {
	//slaveSelectHigh();

	spi_deselect_device(&SPIE, &spi_device_conf);

	radio_enable_interrupt();//interrupts();
}

// ON  = disable filtering to capture all frames on network
// OFF = enable node/broadcast filtering to capture only frames sent to this/broadcast address
void RFM69::promiscuous(bool onOff) {
	_promiscuousMode = onOff;
	//writeReg(REG_PACKETCONFIG1, (readReg(REG_PACKETCONFIG1) & 0xF9) | (onOff ? RF_PACKET1_ADRSFILTERING_OFF : RF_PACKET1_ADRSFILTERING_NODEBROADCAST));
}

void RFM69::setHighPower(bool onOff) {
	_isRFM69HW = onOff;
	writeReg(REG_OCP, _isRFM69HW ? RF_OCP_OFF : RF_OCP_ON);
	if (_isRFM69HW) // turning ON
		writeReg(REG_PALEVEL, (readReg(REG_PALEVEL) & 0x1F) | RF_PALEVEL_PA1_ON | RF_PALEVEL_PA2_ON); // enable P1 & P2 amplifier stages
	else
		writeReg(REG_PALEVEL, RF_PALEVEL_PA0_ON | RF_PALEVEL_PA1_OFF | RF_PALEVEL_PA2_OFF | _powerLevel); // enable P0 only
}

void RFM69::setHighPowerRegs(bool onOff) {
	writeReg(REG_TESTPA1, onOff ? 0x5D : 0x55);
	writeReg(REG_TESTPA2, onOff ? 0x7C : 0x70);
}

void RFM69::setCS(uint8_t newSPISlaveSelect) {

}

// for debugging
void RFM69::readAllRegs()
{
	uint8_t regVal;

	for (uint8_t regAddr = 1; regAddr <= 0x4F; regAddr++)
	{
		regVal = readReg(regAddr);
		//cdc_write_string("Address: 0x");
		//cdc_write_hex(regAddr);
		//cdc_log_hex(" : Value 0x", regVal);
	}
}

uint8_t RFM69::readTemperature(uint8_t calFactor) // returns centigrade
{
	setMode(RF69_MODE_STANDBY);
	writeReg(REG_TEMP1, RF_TEMP1_MEAS_START);
	while ((readReg(REG_TEMP1) & RF_TEMP1_MEAS_RUNNING));
	return ~readReg(REG_TEMP2) + COURSE_TEMP_COEF + calFactor; // 'complement' corrects the slope, rising temp = rising val
} // COURSE_TEMP_COEF puts reading in the ballpark, user can add additional correction

void RFM69::rcCalibration()
{
	writeReg(REG_OSC1, RF_OSC1_RCCAL_START);
	while ((readReg(REG_OSC1) & RF_OSC1_RCCAL_DONE) == 0x00);
}
