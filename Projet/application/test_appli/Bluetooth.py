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

    def send_flag(self, x,y):
        data = "%d%d%d%d" % (3,int(x),int(y),1)
        data = struct.pack('BBBB',int(data[0]),int(data[1]),int(data[2]),int(data[3]))
        print "Sending flag"
        self.sock.send(data)

    def send_reposition(self, x, y, dire):
        data = "%d%d%d%d" % (4,int(x),int(y),int(dire))
        data = struct.pack('BBBB',int(data[0]),int(data[1]),int(data[2]),int(data[3]))
        print "Sending reposition"
        self.sock.send(data)

    def send_move(self, l):
        if len(l) == 2:
            data = "%d%d%d%d" % (1,l[0],l[1],1)
            data = struct.pack('BBB',int(data[0]),int(data[1]),int(data[2]),int(data[3]))
            print "Sending move"
            print "Send move ",data, "(%d,%d,%d)" % (l[0],l[1],1)
            self.sock.send(data)
            print "Waiting ACK"
            data=self.sock.recv(4)
            l=struct.unpack('BBBB',data)
            while l[0] != 7:
                data=self.sock.recv(4)
            print "ACK Receive"
#           wait ack format : "7 1 1 1"
        else:
            print "Error send message"
