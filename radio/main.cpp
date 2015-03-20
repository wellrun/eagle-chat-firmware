extern "C" {
	#include "asf.h"
	#include <avr/io.h>
	#include "cdc.h"
	#include <util/delay.h>
}
#include "RFM69.h"


#define NODEID        1    //unique for each node on same network
#define NETWORKID     100  //the same on all nodes that talk to each other
#define GATEWAYID     5
#define TOID          1
//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
//#define FREQUENCY   RF69_433MHZ
#define FREQUENCY   RF69_868MHZ
//#define FREQUENCY     RF69_915MHZ
//#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
#define ACK_TIME      1000 // max # of ms to wait for an ack
#define NUM_RETRIES   5

int TRANSMITPERIOD = 150; //transmit a packet to gateway so often (in ms)
char payload[] = "123 ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char buff[20];
char sendSize=0;
bool requestACK = true;
RFM69 radio;

int timethen = rtc_get_time();

int main (void)
{
	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	ioport_init();

	rtc_init();

	cdc_start();
	_delay_ms(10);


	while (!cdc_opened());
	cdc_log_int("About to instantiate module ", rtc_get_time());
	radio = RFM69();

	cdc_log_int("About to intialize module ", rtc_get_time());
	radio.initialize(FREQUENCY,NODEID,NETWORKID);

	cdc_log_int("Initialized: ", rtc_get_time());

	radio.setHighPower();
	radio.setPowerLevel(31);
	radio.promiscuous(true);

	uint8_t mode = 0;
	while (mode != 'S' && mode != 'R') {
		cdc_write_string("Enter S for send or R for receive: ");
		mode = udi_cdc_getc();
	}


	radio.readAllRegs();

	if (mode == 'S') {
		while (1) {
			cdc_write_line("Send mode.");
	        if(radio.sendWithRetry(TOID, payload, 30, NUM_RETRIES, 4000)){
	             cdc_log_int("I think I sent something ", (uint32_t)radio.RSSI);
	        } else {
	        	cdc_log_int("I sent but I didn't recieve an ACK ", rtc_get_time());
	        }
		}
	} else {
		_delay_ms(10000);
		while(1) {
	        if (radio.receiveDone())
	        {
	            //cdc_write_line("RECEIVED");
	            cdc_log_int("Recieved Packet from ", radio.SENDERID);
	            cdc_write_line("Message: ");
	            for (char i = 0; i < radio.DATALEN; i++)
	                udi_cdc_putc((char)radio.DATA[i]);
	            cdc_newline();
	            cdc_log_int(" RX_RSSI: -", radio.RSSI);

	            if (radio.ACKRequested() && mode == 'R')
	            {
	                cdc_write_line("ACK requested");
	                radio.sendACK("ACK", 3);
	                cdc_write_line(" - ACK sent");
	                //cdc_log_int("RX_RSSI: ", (uint32_t)radio.RSSI);
	            }
	        }
		}
    }
	while (1);
}
