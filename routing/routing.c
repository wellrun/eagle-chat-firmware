#include "routing_config.h"
#include "routing.h"
#include "packet_fifo.h"
#include "radio.h"
#include <string.h>

static uint8_t framePayload[MAX_PAYLOAD_SIZE];

static RoutingTableEntry routingTable[ROUTING_TABLE_ENTRIES];

static uint8_t _nodeId = 0;

static RRQProgress rrqProgress;

static packet_fifo_t sendFifo;

static packet_fifo_t receiveFifo;

RRQProgress * getRrqProgress() {
	return &rrqProgress;
}


#define RECENT_RRQ_NUM      20
static uint8_t recentRRQ[RECENT_RRQ_NUM];
static uint8_t recentRRQ_p;


RoutingTableEntry * getNextHop(uint8_t dest);
bool forward(RoutingTableEntry *nextHopEntry, uint8_t *framePayload, uint8_t payloadLen);

static void logRecentRRQ(uint8_t n);
// static void trashRecentRRQs();

bool didRecentlyForwardRRQ(uint8_t id);

static inline bool entryIsValid(RoutingTableEntry * entry) {
	return (entry->nextHop != 0) && (entry->failures < MAX_ACK_FAILURES);
}

static inline void invalidateEntry(RoutingTableEntry * entry) {
	entry->nextHop = 0;
	entry->failures = 0;
}

void debugPrintRoutingTable() {
	uint8_t i = 0;
	for (i = 1; i < 255; i ++) {
		debugPrintRoutingTableEntry(i);
	}
}

void debugPrintRoutingTableEntry(uint8_t entry) {

	RoutingTableEntry *r = &routingTable[entry];

	if (entryIsValid(r)) {
		cdc_log_int("Routing table entry for: ", entry);
		cdc_log_int("Next hop: ", r->nextHop);
		cdc_log_int("Failures: ", r->failures);
		cdc_log_int("Original RRQID: ", r->originalRRQID);
	} else {
		//cdc_log_int("No routing entry for node: ", entry);
	}

}

void debugPrintRecentRRQ() {
	uint8_t i = 0;
	for (i = 0; i < recentRRQ_p; i ++) {
		cdc_log_int("Recent RRQ: ", recentRRQ[i]);
	}
}

void debugPrintRRQProgress() {
	cdc_write_line("RRQ Progress info");
	cdc_log_int("Dest: ", rrqProgress.dest);
	cdc_write_string("Timestamp: ");
	cdc_write_int(rrqProgress.timestamp);
	cdc_newline();
	if (rrqProgress.resolved) {
		cdc_write_line("Resolved.");
	} else {
		cdc_write_line("Not resolved.");
	}
}

void debugPrintPacketHeader(PacketHeader *ph);
void debugPrintPacketHeader(PacketHeader *ph) {
	cdc_write_line("PACKET:");
	cdc_log_int("    Source: ", ph->source);
	cdc_log_int("    Dest: ", ph->dest);
	cdc_write_string("    Type: ");
	switch (ph->type) {
		case PACKET_TYPE_CONTENT:
			cdc_write_string("CONTENT");
			break;
		case PACKET_TYPE_RRQ:
			cdc_write_string("RRQ");
			break;
		case PACKET_TYPE_RUP:
			cdc_write_string("RUP");
			break;
		case PACKET_TYPE_FAIL:
			cdc_write_string("FAIL");
			break;
		case PACKET_TYPE_PUBKEY:
			cdc_write_string("PUBKEY");
			break;
	}
	cdc_newline();
}

void setupRouting(uint8_t nodeId) {

	memset(framePayload, 0, sizeof(framePayload));
	memset(routingTable, 0, sizeof(routingTable)); // init routing table

	packet_fifo_init(&sendFifo);

	setupRadio();
	setRoutingId(nodeId);

	memset(recentRRQ, 0, RECENT_RRQ_NUM);
	recentRRQ_p = 0;
}

void setRoutingId(uint8_t nodeId) {
	_nodeId = nodeId;
	setAddress(nodeId);
}

/*
* Attempts to get the routing table entry for the path to dest
* If an invalid entry is found, returns false
* Entries are invalid if the nextHop field is 0 or ack failures equals or
* exceeds MAX_ACK_FAILURES
*/
RoutingTableEntry * getNextHop(uint8_t dest) {

	RoutingTableEntry *r = &routingTable[dest];

	if (entryIsValid(r)) {
		return r;
	}

	invalidateEntry(r);

	return NULL;
}

bool forward(RoutingTableEntry *nextHopEntry, uint8_t *framePayload, uint8_t frameLen) {

	uint32_t send_time;
	bool acked;
	uint8_t retries = 0;

	while (retries < 5) {
		sendFrame(nextHopEntry->nextHop, framePayload, frameLen);


		acked = false;

		send_time = rtc_get_time();
		while (rtc_get_time() - send_time < ROUTING_ACK_WAIT) {
			if ((acked = ackReceived(nextHopEntry->nextHop))) {
				break;
			}
		}

		if (acked) {                // node is alive
			nextHopEntry->failures = 0;  // reset failure count
			return true;
		}
		nextHopEntry->failures++;
		retries ++;
	}

	return false;
}

bool sendPacket(PacketHeader *h, uint8_t *payload, uint8_t payloadLen) {

	// Treat the packet header as a byte array
	uint8_t *harray = (uint8_t *)h;

	// Put header and payload into framePayload
	memcpy(framePayload, harray, PACKET_HEADER_SIZE);
	memcpy(&framePayload[PACKET_HEADER_SIZE], payload, payloadLen);

	// Try to find the next hop to the destination
	RoutingTableEntry *nextHopEntry = getNextHop(h->dest);

	if (nextHopEntry != NULL) {

		// Attempt to forward the packet
		return forward(nextHopEntry, framePayload, payloadLen + PACKET_HEADER_SIZE);

	} else {

		return false;

	}

}

bool queuePacket(PacketHeader h, uint8_t *payload, uint8_t payloadLen) {

	if (!packet_fifo_isFull(&sendFifo)) {

		packet_fifo_write(&sendFifo, h, payload, payloadLen);

		return true;

	} else {

		return false;

	}
}

void processSendQueue(void) {

	if (!packet_fifo_isEmpty(&sendFifo)) {

		// Grab the PacketHeader
		PacketHeader h = packet_fifo_peekHeader(&sendFifo);

		if (routeExistsTo(h.dest)) {

			uint8_t *payload;
			uint8_t payloadLen;

			// Safe to consume packet
			payloadLen = packet_fifo_read(&sendFifo, &h, &payload);

			cdc_write_line("Sent packet out of send queue");

			// Send the packet
			sendPacket(&h, payload, payloadLen);

		} else if (!routeRequestInProgress()) {

			initiateRouteRequest(h.dest);

		}

	}
}

void logRecentRRQ(uint8_t n) {
	recentRRQ[recentRRQ_p] = n;
	recentRRQ_p++;
	if (recentRRQ_p == RECENT_RRQ_NUM) recentRRQ_p = 0;
}

/*
void trashRecentRRQs() {
	memset(recentRRQ, 0, RECENT_RRQ_NUM);
	recentRRQ_p = 0;
}
*/

bool didRecentlyForwardRRQ(uint8_t id) {
	uint8_t i = 0;
	for (i = 0; i < RECENT_RRQ_NUM; i ++) {
		if (recentRRQ[i] == id) return true;
	}
	return false;
}

void initiateRouteRequest(uint8_t dest) {

	// We want to send out a route request to find a path to dest

	rrqProgress.dest = dest;
	rrqProgress.resolved = false;
	rrqProgress.timestamp = rtc_get_time();
	rrqProgress.active = true;

	PacketHeader h;
	h.source = _nodeId;
	h.dest = dest;
	h.type = PACKET_TYPE_RRQ;

	RRQPacketHeader rh;
	rh.rrqID = (uint8_t) rtc_get_time();
	if (rh.rrqID == 0)
		rh.rrqID = 1;
	rh.hopcount = 0;

	memcpy(framePayload, (uint8_t *)&h, PACKET_HEADER_SIZE);
	memcpy(&framePayload[PACKET_HEADER_SIZE], (uint8_t *)&rh, RRQ_PACKET_HEADER_SIZE);

	// Broadcast it?

	logRecentRRQ(rh.rrqID);

	broadcastFrame(framePayload, PACKET_HEADER_SIZE + RRQ_PACKET_HEADER_SIZE);

}

bool routeExistsTo(uint8_t dest) {

	// First, if a pending RRQ resolved, 'destroy' it

	if (rrqProgress.active) {
		uint32_t since = rtc_get_time() - rrqProgress.timestamp;
		if (rrqProgress.resolved || (since >= 1000)) {
			// If it's active and resolved, OR taking too long, deactivate it
			rrqProgress.active = false;
		}
	}

	return (getNextHop(dest) != NULL) && (rrqProgress.dest != dest || !rrqProgress.active);

}

bool routeRequestInProgress() {
	return rrqProgress.active;
}

bool packetsToRead() {
	return !packet_fifo_isEmpty(&receiveFifo);
}

uint8_t packetReceivedPeek(PacketHeader *h, uint8_t **payload) {
	return packet_fifo_peek(&receiveFifo, h, payload);
}

void packetReceivedSkip() {
	packet_fifo_skip(&receiveFifo);
}

void handleReceived() {

	uint8_t senderNodeId, frameLength;
	bool needsAck;

	while (framesToRead()) {

		// Get the raw frame
		getNextFrame(&senderNodeId, &frameLength, framePayload, &needsAck);

		// Send an ack if needed
		if (needsAck) {
			sendAck(senderNodeId);
		}

		// Unpack the packet header from the raw frame
		PacketHeader *h = (PacketHeader *)framePayload;

		// Unpack the RRQ packet header from the raw frame
		// (This will only be valid data in the case that the packet is an RRQ or a RUP)
		RRQPacketHeader *rh = (RRQPacketHeader *)&framePayload[PACKET_HEADER_SIZE];

		//cdc_log_int("Packet type: ", h->type);

		switch (h->type) {
			case PACKET_TYPE_RRQ:

				if (h->dest == _nodeId) {

					#if FORCE_HOPS
					if (rh->hopcount > 0) { // Ignore RRQs directly from originator

					#endif

						// Update our routing tables
						RoutingTableEntry r;
						r.nextHop = senderNodeId;
						r.failures = 0;
						r.originalRRQID = rh->rrqID;
						routingTable[h->source] = r;
						//cdc_write_string("RRQ: ");

						// Swap source and dest in the packet header
						uint8_t temp = h->source;
						h->source = h->dest;
						h->dest = temp;

						// Change the packet type to a RUP
						h->type = PACKET_TYPE_RUP;

						// Time to get that packet on the ol' dusty trail
						forward(&r, framePayload, frameLength);

					#if FORCE_HOPS
					} else {

						cdc_log_int("Ignoring RRQ directly from: ", h->source);

					}
					#endif


				} else {

					if (!didRecentlyForwardRRQ(rh->rrqID)) {

						uint8_t *nodeList = &framePayload[PACKET_HEADER_SIZE + RRQ_PACKET_HEADER_SIZE];
						nodeList[rh->hopcount] = _nodeId;

						rh->hopcount++;

						broadcastFrame(framePayload, PACKET_HEADER_SIZE + RRQ_PACKET_HEADER_SIZE + rh->hopcount);

						// We re-broadcasted this RRQ
						// Add it to our recently broadcast RRQs
						logRecentRRQ(rh->rrqID);

					}

				}


				break;

			case PACKET_TYPE_RUP:

				// We are the originator of the route request
				if (h->dest == _nodeId && rrqProgress.dest == h->source) {

					//cdc_log_int("RUP: SenderId: ", senderNodeId);

					// Save the next hop in the routing table
					routingTable[rrqProgress.dest].nextHop = senderNodeId; // nextHop;
					routingTable[rrqProgress.dest].originalRRQID = rh->rrqID;

					//cdc_write_line("RUP: Should have updated routing table. Result:");


					// Mark current RRQ as resolved
					rrqProgress.resolved = true;

				} else {

					// Okay team, We got a RUP. It aint for us though.
					// The sender is the next hop for the path to "source" (the sender of the RUP)

					RoutingTableEntry r_source;
					r_source.nextHop = senderNodeId;
					r_source.failures = 0;
					r_source.originalRRQID = rh->rrqID;
					routingTable[h->source] = r_source;

					uint8_t *nodeStack = &framePayload[PACKET_HEADER_SIZE + RRQ_PACKET_HEADER_SIZE];
					uint8_t lastNode;

					if (rh->hopcount >= 2) {

						// The last node on the hop stack should be our address
						// The second to last node on the hop stack is the next hop towards "dest" (the node who originally initiated a route request for this reply)

						lastNode = nodeStack[rh->hopcount - 2];

					} else {

						// "Dest" is our next hop to the destination

						lastNode = h->dest;

					}

					RoutingTableEntry r_dest;
					r_dest.nextHop = lastNode;
					r_dest.failures = 0;
					r_dest.originalRRQID = rh->rrqID;
					routingTable[h->dest] = r_dest;

					// Pop the node stack
					rh->hopcount--;

					// The frame length will in-effect reduce by 1 because of popping the node stack
					forward(&r_dest, framePayload, frameLength - 1);

				}

				break;

			case PACKET_TYPE_FAIL: // An error happened in the routing chain
				routingTable[h->source].nextHop = 0; // invalidate route

				// TODO: Queue failure message for host
				// Include packet id once that's a thing

				break;

			default:

				if (h->dest == _nodeId) { // This packet is for us

					// Store in receiveFifo, accessible from other modules
					packet_fifo_write(	&receiveFifo,
										*h,
										&framePayload[PACKET_HEADER_SIZE],
										frameLength - PACKET_HEADER_SIZE);
					continue;
				}

				// Realistically, here we'll want to update our own routing tables
				// with information about h->source

				// Get the next hop to the packet's destination
				RoutingTableEntry *nextHopEntry = getNextHop(h->dest);

				if (nextHopEntry != NULL) {
					
					#ifdef PRINT_ROUTING_LOGS
						cdc_write_line("Routing:");
						debugPrintPacketHeader(h);
					#endif

					forward(nextHopEntry, framePayload, frameLength);
				
				} else {
					// Do a route error

					h->type = PACKET_TYPE_FAIL;

					// Reverse the trajectory
					uint8_t temp = h->dest;
					h->dest = h->source;
					h->source = temp;

					// This should exist, right?
					nextHopEntry = getNextHop(h->dest);

					if (nextHopEntry != NULL) {
						// Try to send the error back to the source
						// If this doesn't work, we can't help you

						forward(nextHopEntry, (uint8_t *)h, PACKET_HEADER_SIZE);
					}
				}

				break;
		}

	}

}
