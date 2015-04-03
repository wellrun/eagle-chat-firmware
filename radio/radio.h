#ifndef RADIO_H_INCLUDED
#define RADIO_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "asf.h"

#define FREQUENCY   	RF69_868MHZ
#define NODEID        	1    //unique for each node on same network
#define NETWORKID     	100  //the same on all nodes that talk to each other

#define MAX_PAYLOAD_SIZE        250

#define OFFSET_TARGET_ADDRESS 	0
#define OFFSET_SENDER_ADDRESS 	1
#define OFFSET_CTL				2
#define OFFSET_LENGTH			3
#define OFFSET_DATA				4

#define MASK_ACK_REQUESTED		0x40

//! Configures the radio for use
void setupRadio(void);

//! Sets the radio's "MAC" address on the network
/*!
  \param address    The address
*/
void setAddress(uint8_t address);

//! Sends a packet indiscriminately to the broadcast address
/*!
  \param packet     Byte array
  \param size       Length of array no greater than MAX_PAYLOAD_SIZE
*/
void broadcastFrame(uint8_t *packet, uint8_t size);

//! Sends a packet to a particular address
/*!
  \param to_address Destination
  \param packet     Byte array
  \param size       Length of array no greater than MAX_PAYLOAD_SIZE
*/
void sendFrame(uint8_t to_address, uint8_t *frame, uint8_t size);

//! Sends an ACK packet to a particular address
/*!
  \param to_address The destination
*/
void sendAck(uint8_t to_address);

//! Checks if an ACK packet has been receieved from a particular address
/*!
  \param from_address   Node from which an ACK is expected
*/
bool ackReceived(uint8_t from_address);

//! Returns true if there are unread packets that have been received
bool framesToRead(void);

//! Fills out the arguments with the data of the next packet to be serviced
/*!
  \param senderId   Will be set to the sender's address
  \param length     Will be set to the length of the data payload
  \param buf        Will be filled with the packet's payload (should be at least MAX_PAYLOAD_SIZE bytes)
  \param needsAck   Will be set to true if the packet requested an ACK. False otherwise.
*/
void getNextFrame(uint8_t *senderId, uint8_t *length, uint8_t *buf, bool *needsAck);

#ifdef __cplusplus
}
#endif

#endif /*RADIO_H_INCLUDED*/
