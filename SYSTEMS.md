Software subsystems
===========================

	1. Radio driver and routing stack
	2. Packet protocol stack
	3. Message crypto stack
	4. Application layer
	5. USB stack

### Radio driver and routing stack <a name="radio"></a>

This system talks to the radio transciever and is the final software layer between messages going in and out and the radio.
It accepts packetized data to send and delivers received packet data to the [Packet protocol stack](#packet).

### Packet protocol stack <a name="packet"></a>

This system takes message data to send and parcels it out into network ready packets, providing information on reassembly to the complete message. This may include packet sequence numbers and CRCs/hashes.

Also, the packet protocol stack does the reverse, taking raw packet data delivered from the [radio stack](#radio) and reassembling it into the full message to deliver to either the [application layer](#application) or the [message crypto](#crypto) stack.

### Message crypto stack <a name="crypto"></a>

This system encrypts and decrypts messages. It handles the private storage of keys, and uses them to "box" messages, suitable for sending on to the [packet protocol](#packet) system.

### Application layer <a name="application"></a>

This system is the king of them all, integrating all the other subsystems. It communicates with the host Android or PC using the [usb stack](#usb), and decides when to generate keys, send messages, etc.

### USB stack <a name="usb"></a>

And finally, this system send and receives data to the host device over USB. This module includes the USB driver as well as auxiliary functions like call backs on message reception and so on.


