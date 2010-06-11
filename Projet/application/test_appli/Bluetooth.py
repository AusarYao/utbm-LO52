#!/usr/bin/env python
# -*- coding:Utf-8 -*-

import bluetooth
import struct
import time

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
        self.isconnected = False
        self.sock=bluetooth.BluetoothSocket(bluetooth.RFCOMM)
        print "Init BT"

    def connect(self):
        """Connection"""
        try : self.sock.connect((self.bd_addr, self.port))
        except:
            self.isconnected = False
            print "Unable to connect to device"
        else:
            self.isconnected = True
            print "Connected"

    def disconnect(self):
        """Disconnection"""
        self.isconnected = False
        print "Disconnected"

    def isconnect(self):
        """return true if bluetooth is connected"""
        return self.isconnected

    def recept(self):
        """Recept a string data"""
        data = ''
        while data == '':
            data=self.sock.recv(4)
            #data = '\x02\x01\x01\x02'
        l =struct.unpack('BBBB',data)
        print "Waiting message"        
        print "Recept data, l:",l," ; data:",data
        return l


    def ack(self): ######################### N'AFFICHE PAS DATA
        data = struct.pack('BBBB',98,98,98,98)
      #  self.sock.send(data)
        print "Sending ACK"
        #time.sleep(1)
        print "Ack : ",data

    def send_move(self, l):
        if len(l) == 2:
            data = "%d%d%d" % (l[0],l[1],1)
            data = struct.pack('BBB',int(data[0]),int(data[1]),int(data[2]))
            print "Sending move"
            time.sleep(1)
            print "Send move ",data, "(%d,%d,%d)" % (l[0],l[1],1)
            print "Waiting ACK"
            time.sleep(2)
            print "ACK Receive"
            self.sock.send(data)
#           wait ack format : "7 1 1 1"
        else:
            print "Error send message"
