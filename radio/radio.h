#ifndef RADIO_H_INCLUDED
#define RADIO_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
 
#include "asf.h"

#define FREQUENCY   	RF69_868MHZ
#define NODEID        	1    //unique for each node on same network
#define NETWORKID     	100  //the same on all nodes that talk to each other

#define OFFSET_TARGET_ADDRESS 	0
#define OFFSET_SENDER_ADDRESS 	1
#define OFFSET_CTL				2
#define OFFSET_LENGTH			3
#define OFFSET_DATA				4


void setupRadio(void);

void broadcastPacket(uint8_t *packet, uint8_t size);

bool packetsToRead(void);

void getNextPacket(uint8_t *senderId, uint8_t *length, uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif /*RADIO_H_INCLUDED*/