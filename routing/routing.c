#include "routing.h"
#include "radio.h"
#include <string.h>

static uint8_t framePayload[MAX_PAYLOAD_SIZE];

static RoutingTableEntry routingTable[ROUTING_TABLE_ENTRIES];

static uint8_t _nodeId = 0;

static RRQProgress rrqProgress;

RRQProgress * getRrqProgress() {
    return &rrqProgress;
}


#define RECENT_RRQ_NUM      20
static uint8_t recentRRQ[RECENT_RRQ_NUM];
static uint8_t recentRRQ_p;


bool getNextHop(uint8_t dest, RoutingTableEntry **nextHopEntry);
bool forward(RoutingTableEntry *nextHopEntry, uint8_t *framePayload, uint8_t payloadLen);

static void logRecentRRQ(uint8_t n);
static void trashRecentRRQs(void);
bool didRecentlyForwardRRQ(uint8_t id);

static inline bool entryIsValid(RoutingTableEntry * entry) {
    return (entry->nextHop != 0) && (entry->failures < MAX_ACK_FAILURES);
}

void debugPrintRoutingTable(uint8_t entry) {
    
    RoutingTableEntry *r = &routingTable[entry];

    if (entryIsValid(r)) {
        cdc_log_int("Routing table entry for: ", entry);
        cdc_log_int("Next hop: ", r->nextHop);
        cdc_log_int("Failures: ", r->failures);
        cdc_log_int("Original RRQID: ", r->originalRRQID);
    } else {
        cdc_write_line("No routing entry for that node");
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

void setupRouting(uint8_t nodeId) {

    _nodeId = nodeId;

    memset(framePayload, 0, sizeof(framePayload));
    memset(routingTable, 0, sizeof(routingTable)); // init routing table

    // routingTable[1].nextHop = 1; // Dummify the routing table
    // routingTable[2].nextHop = 2;

    setupRadio();
    setAddress(nodeId);

    memset(recentRRQ, 0, RECENT_RRQ_NUM);
    recentRRQ_p = 0;
}

/*
* Attempts to get the routing table entry for the path to dest
* If an invalid entry is found, returns false
* Entries are invalid if the nextHop field is 0 or ack failures equals or
* exceeds MAX_ACK_FAILURES
*/
bool getNextHop(uint8_t dest, RoutingTableEntry **nextHopEntry) {

    RoutingTableEntry *r = &routingTable[dest];

    if (entryIsValid(r)) {

        *nextHopEntry = r;
        return true;

    }

    return false;
}

bool forward(RoutingTableEntry *nextHopEntry, uint8_t *framePayload, uint8_t frameLen) {

    uint32_t send_time;
    bool acked;
    uint8_t retries = 0;

    while (retries < 5) {
        sendFrame(nextHopEntry->nextHop, framePayload, frameLen);


        acked = false;

        send_time = rtc_get_time();
        while (rtc_get_time() - send_time < 50) {
            if ((acked = ackReceived(nextHopEntry->nextHop))) {
                break;
            }
        }

        if (acked) {                // node is alive
            nextHopEntry->failures = 0;  // reset failure count
            return true;
        }

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
    RoutingTableEntry *nextHopEntry;
    bool entryFound = getNextHop(h->dest, &nextHopEntry);

    if (entryFound) {

        // Attempt to forward the packet
        return forward(nextHopEntry, framePayload, payloadLen + PACKET_HEADER_SIZE);

    } else {

        return false;

    }

}

void logRecentRRQ(uint8_t n) {
    recentRRQ[recentRRQ_p] = n;
    recentRRQ_p++;
    if (recentRRQ_p == RECENT_RRQ_NUM) recentRRQ_p = 0;
}

void trashRecentRRQs() {
    memset(recentRRQ, 0, RECENT_RRQ_NUM);
    recentRRQ_p = 0;
}

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

    PacketHeader h;
    h.source = _nodeId;
    h.dest = dest;
    h.flags = 0;
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

        cdc_log_int("Packet type: ", h->type);

        switch (h->type) {
            case PACKET_TYPE_CONTENT:

                if (h->dest == _nodeId) { // This packet is for us
                    // Should probably stick in a buffer as needed
                    cdc_write_line("Got packet for us");
                    cdc_log_string("Payload: ", &framePayload[PACKET_HEADER_SIZE]);
                    continue;
                }

                // Realistically, here we'll want to update our own routing tables
                // with information about h->source

                // Get the next hop to the packet's destination
                RoutingTableEntry *nextHopEntry;
                bool entryFound = getNextHop(h->dest, &nextHopEntry); // pointers having pointers, such a shame

                if (entryFound) {
                    forward(nextHopEntry, framePayload, frameLength);
                } else {
                    // Do a route error
                }

                break;

            case PACKET_TYPE_RRQ:

                if (h->dest == _nodeId) {
                    
                    // Update our routing tables
                    RoutingTableEntry r;
                    r.nextHop = senderNodeId;
                    r.failures = 0;
                    r.originalRRQID = rh->rrqID;
                    routingTable[h->source] = r;
                    cdc_write_string("RRQ: ");
                    debugPrintRoutingTable(h->source);

                    // Swap source and dest in the packet header
                    uint8_t temp = h->source;
                    h->source = h->dest;
                    h->dest = temp;

                    // Change the packet type to a RUP
                    h->type = PACKET_TYPE_RUP;

                    // Time to get that packet on the ol' dusty trail
                    forward(&r, framePayload, frameLength);


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

                    cdc_log_int("RUP: SenderId: ", senderNodeId);
                    // Save the next hop in the routing table
                    //uint8_t nextHop = framePayload[PACKET_HEADER_SIZE + RRQ_PACKET_HEADER_SIZE];
                    routingTable[rrqProgress.dest].nextHop = senderNodeId; // nextHop;
                    routingTable[rrqProgress.dest].originalRRQID = rh->rrqID;

                    cdc_write_line("RUP: Should have updated routing table. Result:");
                    debugPrintRoutingTable(rrqProgress.dest);

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

        }

    }

}
