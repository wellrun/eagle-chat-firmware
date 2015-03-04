/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

extern "C" {
	#include "asf.h"
	#include <avr/io.h>
	#include "cdc.h"
    #include <util/delay.h>
}
#include "RFM69.h"

#define NODEID        2    //unique for each node on same network
#define NETWORKID     100  //the same on all nodes that talk to each other
#define GATEWAYID     1
#define TOID          3
//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
//#define FREQUENCY   RF69_433MHZ
#define FREQUENCY   RF69_868MHZ
//#define FREQUENCY     RF69_915MHZ
//#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
#define ACK_TIME      30 // max # of ms to wait for an ack

#define UDI_CDC_DEFAULT_RATE             9600 
#define F_CPU                           32000000UL

int TRANSMITPERIOD = 150; //transmit a packet to gateway so often (in ms)
char payload[] = "123 ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char buff[20];
char sendSize=0;
bool requestACK = false;
RFM69 radio;

int main (void)
{   	
    cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

    ioport_init();

	rtc_init();

	cdc_start();
    _delay_ms(10);
    while (udi_cdc_getc() != '2'){
    cdc_log_int("About to instantiate module ", rtc_get_time());}
    radio = RFM69();
	while (udi_cdc_getc() != '3'){
    cdc_log_int("About to intialize module ", rtc_get_time());}
    radio.initialize(FREQUENCY,NODEID,NETWORKID);
	cdc_log_int("Initialized: ", rtc_get_time());
    //radio.setHighPower();
/*
    if(radio.sendWithRetry(TOID, payload, 30, requestACK)){
        cdc_log_int("Holyshit I think I sent something ", rtc_get_time());
    } else {
        cdc_log_int("I didn't send anything because something borked ", rtc_get_time());
    }
*/      

	radio.readAllRegs();

    while(1);
}
