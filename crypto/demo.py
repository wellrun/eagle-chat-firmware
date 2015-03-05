import serial
import binascii as bn

if __name__ == '__main__':

	snum = int(raw_input("Serial port: "))

	ser = serial.Serial(snum - 1);

	print "Asking board to begin encryption process"

	ser.write('e')

	lines1 = []
	for i in range(5):
		lines1.append(ser.readline())

	print "Board said: "

	for line in lines1:
		print "====> " + line

	message = raw_input("Enter a message to tell to the board: ")

	ser.write(message)
	ser.write("\n")

	lines2 = []
	for i in range(3):
		lines2.append(ser.readline())

	print "Board said: "
	for line in lines2:
		print "====> " + line

	aPrivate = bn.unhexlify(lines1[1].split(':')[1].strip(" \n"))
	aPublic = bn.unhexlify(lines1[2].split(':')[1].strip(" \n"))
	bPrivate = bn.unhexlify(lines1[3].split(':')[1].strip(" \n"))
	bPublic = bn.unhexlify(lines1[4].split(':')[1].strip(" \n"))

	nonce = bn.unhexlify(lines2[1].split(':')[1].strip(" \n"))
	encrypted = bn.unhexlify(lines2[2].split(':')[1].strip(" \n"))


	ser.close();


	print "Now, we decrypt on another board. Plug another board in."

	snum = int(raw_input("Serial port: "))

	ser = serial.Serial(snum - 1);

	print "Asking board to begin decryption process"

	ser.write('d')

	ser.write(aPublic)
	ser.write(bPrivate)
	ser.write(encrypted)
	ser.write("\n")
	ser.write(nonce)

	response = ser.readline()

	print "Board said: "
	print "====> " + response

	decrypted = response.split(':')[1].strip(" \n")

	print "Decrypted message: " + decrypted

