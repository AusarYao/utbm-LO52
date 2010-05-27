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

        #Dessin
        self.bt = BT.BT()
        self.game = G.Game(3,3)
        self.canvas = Canvas.MyCanvas(self.interface.get_object("canvas"),self.game)

        self.interface.connect_signals(self)

        self.game.m.print_map()
        self.game.f.add_flag(2,2)
#        self.game.f.init_flag()

    def on_btnConnect_clicked(self, widget):
        btnCo = self.interface.get_object("btnConnect")
        if not btnCo.get_active():
            btnCo.set_label("Connection")
            self.log.add("Disconnected")
        else:
            btnCo.set_label("Disconnection")
            self.log.add("Connected")

    def on_btnReset_clicked(self,widget):
        self.log.add("I'm useless")
        self.on_canvas_expose_event()

    def on_btnRobot_clicked(self, widget):
        x = self.robot_x.get_text()
        y = self.robot_y.get_text()
        if int(x) < self.game.m.mapw and int(x) >= 0 and \
           int(y) < self.game.m.maph and int(y) >= 0:
            self.game.r.update(int(x), int(y))
            self.on_canvas_expose_event()
            self.log.add("New position for the robot("+x+","+y+")")
        else:
            self.log.add("Error new position robot")

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
