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

gtk.gdk.threads_init()

class ThreadCanvas(T.Thread):
    """Thread to display the map"""
    def __init__(self, interface):
        T.Thread.__init__(self)
        self.interface = interface
        self.stopevent = T.Event()
        self.start()

    def run(self):
        while not self.stopevent.isSet():
            self.interface.on_canvas_expose_event()
            #FPS
            self.stopevent.wait(0.1)
        print "ThreadCanvas is closed properly"

    def stop(self):
        self.stopevent.set()

class ThreadBluetooth(T.Thread):
    """Thread for the BT"""
    def __init__(self, interface):
        T.Thread.__init__(self)
        self.bt = BT.BT()
        self.interface = interface
        self.stopevent = T.Event()
        self.start()

    def go(self):
        self.bt.connect()

    def ungo(self):
        self.bt.disconnect()

    def stop(self):
        self.stopevent.set()

    def run(self):
        while not self.stopevent.isSet():
            if self.bt.isconnected:
                bt_m = self.bt.recept()
                s = "%s" % (bt_m,)
                self.interface.log.add("Message receive :"+s)
                self.interface.game.update_explore(bt_m)
                self.bt.ack()
                self.stopevent.wait(0.01)
        print "ThreadBluetooth is closed properly"

    def send(self, data):
        self.bt.send_move(data)

    def isconnect(self):
        return self.bt.isconnect()


class ThreadAlgo(T.Thread):
    """Thread to run the algo"""
    def __init__(self, interface):
        T.Thread.__init__(self)
        self.interface = interface
        self.started = T.Event()
        self.waiting = True
        self.start()

    def run(self):
        while not self.started.isSet():
            self.started.wait(0.001)
            first_connected = True
            first_guidage = True
            while not self.waiting:
                time.sleep(0.001)
                if self.interface.bt.isconnect():
                    if self.interface.mode == 1:
                        ldata = self.interface.game.find_next_move()
                        if ldata != ():
                            first_guidage = True
                            s = "%s" %(ldata,)
                            self.interface.log.add("Next move:"+s)
                            self.interface.bt.send(ldata)
                            self.interface.game.update_guiding(ldata[0],ldata[1])
                        elif first_guidage:
                            first_guidage = False
                            self.interface.log.add("No direction")
                elif first_connected:
                    self.interface.log.add("Not connected")
                    first_connected = False
        print "ThreadAlgo is closed properly"

    def wait(self):
        self.waiting = True

    def notify(self, interface):
        self.waiting = False
        self.interface = interface

    def stop(self):
        self.waiting = True
        self.started.set()


class Interface(object):
    """Interface for the robot"""
    def __init__(self):
        self.interface = gtk.Builder()
        self.interface.add_from_file("interface.ui")

        self.robot_x= self.interface.get_object("robotX")
        self.robot_y= self.interface.get_object("robotY")

        #Log control
        self.log = Log.MyLog(self.interface.get_object("log"),self.interface.get_object("scrollLog"))

        #Init the game, connection
        self.game = G.Game(12,6)
        self.mode = 0

        #Dessin
        self.canvas = Canvas.MyCanvas(self.interface.get_object("canvas"),self.game)

        #Thread
        self.algo = ThreadAlgo(self)
        self.render = ThreadCanvas(self)
        self.bt = ThreadBluetooth(self)

        self.game.f.add_flag(4,2)
        self.game.f.add_flag(5,5)
        self.game.f.init_flag()
        self.game.m.add_wall(1,1,2)
        self.game.m.add_wall(1,1,4)


        self.combo =  self.interface.get_object("comboFlag")
        self.interface.connect_signals(self)
        self.updateCombo()

    def on_btnConnect_clicked(self, widget):
        btnCo = self.interface.get_object("btnConnect")
        if not btnCo.get_active():
            btnCo.set_label("Connection")
            self.log.add("Disconnected")
            self.bt.ungo()
        else:
            btnCo.set_label("Disconnection")
            self.log.add("Connected")
            self.bt.go()

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

    def on_btnInit_clicked(self, widget):
        self.log.add("Map initialization (flag no taken)")
        self.game.f.init_flag()

    def on_btnNotTaken_clicked(self, widget):
        value = self.get_active_text(self.combo)
        lst = value.split(';')
        lst2 = [int(lst[x]) for x in xrange(len(lst))]
        if self.game.f.mapdata[(lst2[0],lst2[1])]:
            self.game.f.mapdata[(lst2[0],lst2[1])] = False
        else:
            self.game.f.mapdata[(lst2[0],lst2[1])] = True
        self.log.add(("Flag (%d,%d) status has changed")%(lst2[0],lst2[1]))

    def get_active_text(self, combobox):
        model = combobox.get_model()
        active = combobox.get_active()
        if active < 0:
            return None
        return model[active][0]


    def updateCombo(self):
        list_store = gtk.ListStore(gobject.TYPE_STRING)
        L = self.game.f.list_flag()
        for x in xrange(len(L)):
            list_store.append([str(L[x][0])+";"+str(L[x][1])])
        self.combo.set_model(list_store)
        self.combo.set_active(0)
        cell = gtk.CellRendererText()
        self.combo.pack_start(cell, True)
        self.combo.add_attribute(cell, "text", 0)


    def on_btnRobot_clicked(self, widget):
        x = self.robot_x.get_text()
        y = self.robot_y.get_text()
        if x.isdigit() and y.isdigit():
            if int(x) < self.game.m.mapw and int(x) >= 0 and \
               int(y) < self.game.m.maph and int(y) >= 0:
                self.game.r.update(int(x), int(y))
                self.log.add("New position for the robot("+x+","+y+")")
            else:
                self.log.add("Error new position robot")
        else:
            self.log.add("Value are not integer")

    def on_btnResetLog_clicked(self,widget):
        self.log.clean()

    def on_interface_destroy(self, widget):
        print "Destroying threads..."
        self.render.stop()
        self.algo.stop()
        self.bt.stop()
        self.render.join()
        self.algo.join()
        self.bt.join()
        gtk.main_quit()

    def on_canvas_expose_event(self, * args):
        gtk.gdk.threads_enter()
        self.canvas.update(self.game)
        gtk.gdk.threads_leave()

    def main(self):
        gtk.main()

if __name__ == "__main__":
    g = Interface()
    g.main()
