#ifndef RADIO_H_INCLUDED
#define RADIO_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
 
#include "asf.h"
 
#ifdef __cplusplus
}
#endif

#define FREQUENCY   	RF69_868MHZ
#define NODEID        	1    //unique for each node on same network
#define NETWORKID     	100  //the same on all nodes that talk to each other

void setupRadio(void);

void broadcastPacket(uint8_t *packet, uint8_t size);

bool packetsToRead(void);

void getNextPacket(uint8_t *senderId, uint8_t *length, uint8_t *buf);

#endif /*RADIO_H_INCLUDED*/