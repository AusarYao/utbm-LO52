import bluetooth

#adresse mac du nxt
bd_addr = "00:16:53:09:D7:98"

#port a utiliser avec le nxt
port = 1
sock=bluetooth.BluetoothSocket(bluetooth.RFCOMM)

#defini la socket non bloquante
sock.setblocking(0)

#taille des messages envoyes
MSG_SIZE =4;

def connect():
	sock.connect((bd_addr, port))

def recept():
		data=sock.recv(1024)
		print data
		traiteMessage(data)


def avance():
	sock.send("dddd")

def droite():
	sock.send("d\x00dd")

def gauche():
	sock.send("\x00ddd")

#affiche le message et la position du robot
def traiteMessage(message):
	for i in range(0, len(message), MSG_SIZE):
		messageType=ord(message[i])
		if messageType==0:
			print 'Mur a droite (' + str(ord(message[i+1])) + ',' + str(ord(message[i+2])) + ')'
		elif messageType==1:
			print 'Mur en face (' + str(ord(message[i+1])) + ',' + str(ord(message[i+2]))+ ')'
		elif messageType==2:
			print 'Drapeau (' + str(ord(message[i+1])) + ',' + str(ord(message[i+2]))+ ')'
		elif messageType==3:
			print 'Position courante (' + str(ord(message[i+1])) + ',' + str(ord(message[i+2]))+ ')'
