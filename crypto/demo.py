import serial
import binascii as bn

if __name__ == '__main__':

	print "Plug board 'A' in. This will be the 'sender'"

	snum = int(raw_input("Serial port: "))

	ser = serial.Serial(snum - 1);

	print "Asking board A to generate keys for the demo and start the encryption process"

	ser.write('e')

	lines1 = []
	for i in range(5):
		lines1.append(ser.readline())

	print "Board said: "

	for line in lines1:
		print "====> " + line.strip("\n")

	message = raw_input("Enter a message to tell to the board: ")

	ser.write(message)
	ser.write("\n")

	lines2 = []
	for i in range(3):
		lines2.append(ser.readline())

	print "Board said: "
	for line in lines2:
		print "====> " + line.strip("\n")

	aPrivate = bn.unhexlify(lines1[1].split(':')[1].strip(" \n"))
	aPublic = bn.unhexlify(lines1[2].split(':')[1].strip(" \n"))
	bPrivate = bn.unhexlify(lines1[3].split(':')[1].strip(" \n"))
	bPublic = bn.unhexlify(lines1[4].split(':')[1].strip(" \n"))

	nonce = bn.unhexlify(lines2[1].split(':')[1].strip(" \n"))
	encrypted = bn.unhexlify(lines2[2].split(':')[1].strip(" \n"))


	ser.close();


	print "Now plug in board 'B'. This will be the 'receiver'"

	snum = int(raw_input("Serial port: "))

	ser = serial.Serial(snum - 1);

	print "Asking board B to begin decryption process"

	ser.write('d')

	print "Telling the board A's public key ..."
	ser.write(aPublic)

	print "Telling the board it's own private key ..."
	ser.write(bPrivate)

	print "Telling the board the encrypted message ..."
	ser.write(encrypted)
	ser.write("\n")

	print "Telling the board the nonce used to encrypt the message ..."
	ser.write(nonce)

	response = ser.readline()

	print "Board said: "
	print "====> " + response

	decrypted = response.split(':')[1].strip(" \n")

	print "Decrypted message: " + decrypted

