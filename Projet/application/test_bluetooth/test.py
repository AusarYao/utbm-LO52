import bluetooth

bd_addr = "00:16:53:09:D7:98"
port = 1
sock=bluetooth.BluetoothSocket(bluetooth.RFCOMM)


def connect():
	sock.connect((bd_addr, port))

def recept():
		data=sock.recv(1024)
		print data


def avance():
	sock.send("dd")

def droite():
	sock.send("d\x00")

def gauche():
	sock.send("\x00d")

