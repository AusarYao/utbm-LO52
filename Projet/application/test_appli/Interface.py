#!/usr/bin/env python

# -*- coding:Utf-8 -*-

import pygtk
pygtk.require("2.0")
import gtk
import Canvas
import Log
import Bluetooth as BT
import Game as G
import threading as T
import time
import gobject
import pdb

gobject.threads_init()

verrou = T.Lock()

class Interface(object):
    class StartAlgo(T.Thread):
        """Thread to run the algo"""
        def __init__(self, interface):
            T.Thread.__init__(self)
            self.interface = interface
            self.started = True
            self.waiting = True
            self.start()

        def run(self):
            while self.started:
                time.sleep(0.001)
                while not self.waiting:
                    time.sleep(0.001)
                    if self.interface.bt.isconnected:
                        print "\n########## New time ########"
                        if self.interface.mode == 0:
                            print "Exploration"
                            bt_m = self.interface.bt.recept()
                            s = "%s" % (bt_m,)
                            self.interface.log.add("Message receive :"+s)
                            self.interface.game.update(bt_m)
                            self.interface.bt.ack()
                        else:
                            print "Guidage"
                            ldata = self.interface.game.find_next_move()
                            s = "%s" %(ldata,)
                            self.interface.log.add("Next move:"+s)
                            if ldata != []:
                                self.interface.bt.send_move(ldata)
                                self.interface.game.r.update(ldata[0],ldata[1])
                            else:
                                self.interface.log.add("No direction")
                        self.interface.on_canvas_expose_event()
                    else:
                        print "Not connected"

        def wait(self):
            self.waiting = True
            print "stop"

        def notify(self, interface):
            self.waiting = False
            self.interface = interface
            print "restart"

        def stop(self):
            self.waiting = True
            self.started = False


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

        #Thread
        self.algo = self.StartAlgo(self)

        self.game.f.add_flag(4,2)
        self.game.f.add_flag(5,5)
        self.game.f.init_flag()
        self.game.m.add_wall(1,1,2)

    def on_btnConnect_clicked(self, widget):
        btnCo = self.interface.get_object("btnConnect")
        if not btnCo.get_active():
            btnCo.set_label("Connection")
            self.log.add("Disconnected")
            self.bt.isconnected = False
        else:
            btnCo.set_label("Disconnection")
            self.log.add("Connected")
            self.bt.connect()

    def on_btnStart_clicked(self, widget):
        btnStart = self.interface.get_object("btnStart")
        if btnStart.get_active():
            btnStart.set_label("Stop")
            self.log.add("Start")
            #LAUNCH THE ALGO TO RECEIVE OR SEND DATA
            self.algo.notify(self)
        else:
            btnStart.set_label("Start")
            self.log.add("Stop")
            #STOP THE ALGO
            self.algo.wait()

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
                self.log.add("New position for the robot("+x+","+y+")")
                self.on_canvas_expose_event()
            else:
                self.log.add("Error new position robot")
        else:
            self.log.add("Value are not integer")

    def on_btnResetLog_clicked(self,widget):
        self.log.clean()

    def on_interface_destroy(self, widget):
        print "Destroying threads..."
        self.algo.stop()
        self.algo.join()
        self.algo._Thread__stop()
        gtk.main_quit()

    def on_canvas_expose_event(self, *args):
        verrou.acquire()
        self.log.add("Update Canvas")
        st = time.time()
        self.canvas.update(self.game)
        verrou.release()
        et = time.time()
        print "Canvas refresh in : %f times" %(et-st)

    def main(self):
        gtk.main()

if __name__ == "__main__":
    g = Interface()
    g.main()
