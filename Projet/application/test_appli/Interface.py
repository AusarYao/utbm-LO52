#!/usr/bin/env python

# -*- coding:Utf-8 -*-

import pygtk
pygtk.require("2.0")
import gtk
import Canvas
import Log
import Bluetooth as BT
import Game as G

class Interface(object):
    def __init__(self):
        self.interface = gtk.Builder()
        self.interface.add_from_file("interface.glade")

        self.robot_x= self.interface.get_object("robotX")
        self.robot_y= self.interface.get_object("robotY")

        #Log control
        self.log = Log.MyLog(self.interface.get_object("log"),self.interface.get_object("scrollLog"))

        #Init the game, connection
        self.bt = BT.BT()
        self.game = G.Game(12,6)
        self.mode = 0

        #Dessin
        self.canvas = Canvas.MyCanvas(self.interface.get_object("canvas"),self.game)
        self.interface.connect_signals(self)

        self.game.f.add_flag(2,2)
        self.game.f.init_flag()
#        self.game.m.add_wall(1,1,2)

    def on_btnConnect_clicked(self, widget):
        btnCo = self.interface.get_object("btnConnect")
        if not btnCo.get_active():
            btnCo.set_label("Connection")
            self.log.add("Disconnected")
            #BT.CONNECT()
        else:
            btnCo.set_label("Disconnection")
            self.log.add("Connected")
            #BT.DISCONNECT()

    def on_btnStart_clicked(self, widget):
        btnStart = self.interface.get_object("btnStart")
        if not btnStart.get_active():
            btnStart.set_label("Start")
            self.log.add("Stop")
            #STOP THE ALGO
        else:
            btnStart.set_label("Stop")
            self.log.add("Start")
            #LAUNCH THE ALGO TO RECEIVE OR SEND DATA
            if self.mode == 0: #exploration
                bt_m = self.bt.recept()
                s = "%s" % (bt_m,)
                self.log.add("Message receive :"+s)
                self.game.update(bt_m)
                self.bt.ack()
            elif self.mode == 1: #guiding
                ldata = self.game.find_next_move()
                s = "%s" %(ldata,)
                self.log.add("Next move:"+s)
                if ldata != []:
                    self.bt.send_move(ldata)
                    self.game.r.update(ldata[0],ldata[1])
                else:
                    self.log.add("No direction")
            self.on_canvas_expose_event()

    def on_btnMode_clicked(self, widget):
        btnMode = self.interface.get_object("btnMode")
        if not btnMode.get_active():
            btnMode.set_label("Guiding")
            self.log.add("Exploration mode")
            #SET_MODE(GUIDAGE)
            self.mode = 0
        else:
            btnMode.set_label("Explorate")
            self.log.add("Guiding mode")
            #SET_MODE(EXPLORATION)
            self.mode = 1

    def on_btnRobot_clicked(self, widget):
        x = self.robot_x.get_text()
        y = self.robot_y.get_text()
        if x.isdigit() and y.isdigit():
            if int(x) < self.game.m.mapw and int(x) >= 0 and \
               int(y) < self.game.m.maph and int(y) >= 0:
                self.game.r.update(int(x), int(y))
                self.on_canvas_expose_event()
                self.log.add("New position for the robot("+x+","+y+")")
            else:
                self.log.add("Error new position robot")
        else:
            self.log.add("Value are not integer")

    def on_btnResetLog_clicked(self,widget):
        self.log.clean()

    def on_interface_destroy(self, widget):
        gtk.main_quit()

    def on_canvas_expose_event(self, *args):
        self.canvas.update()

    def main(self):
        gtk.main()

if __name__ == "__main__":
    g = Interface()
    g.main()
