#!/usr/bin/env python
# -*- coding:Utf-8 -*-

import bluetooth

class BT(object):
    """
    Bluetooth class to communicate with the robot
        - connect
        - send
        - receive
    """
    def __init__(self):
        self.bd_addr = "00:16:53:09:D7:98"
        self.port = 1
        self.sock=bluetooth.BluetoothSocket(bluetooth.RFCOMM)
        print "Init BT"


    def connect(self):
        """Connection"""
        try : sock.connect((bd_addr, port))
        except:
            self.isconnected = False
            print "Unable to connect to device"
        else:
            self.isconnected = True
            print "Connected"

    def recept(self):
        """Recept a string data"""
        data=sock.recv(1024)
        print data
        return data

    def ack(self):
        sock.send("ok")
        print "ack"

    def send_move(self,l):
        for x in range(len(l)):
            if l[x]==1:
                self.send_left()
            if l[x]==2:
                self.send_rightt()
            if l[x]==0:
                self.sendGo()

    def send_rightt(self):
        sock.send("d\x00")
        print "Droite"

    def send_left(self):
        sock.send("\x00d")
        print "Gauche"

    def sendGo(self):
        sock.send("dd")
        print "Avance"
