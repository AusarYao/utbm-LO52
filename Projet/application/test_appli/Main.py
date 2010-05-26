# -*- coding:Utf-8 -*-

import Game as G
import Bluetooth as BT

#Exploration(0) or guidage(1)
MODE = 1

#Initialisation de la connection et de la partie
bt = BT.BT()
game = G.Game(3,3)

#Affichage et connection
bt.connect()
game.m.print_map()

#mode exploration
if MODE == 0:
    if (bt.isconnected):
        bt_m = bt.recept()
        game.update(bt_m)
        bt.ack()
    else:
        print "Signal lost"

if MODE == 1:
    game.f.add_flag(2,2)
    game.f.init_flag()
    if (bt.isconnected):
        ldata = game.find_next_move()
        print "Next move:",ldata
        if ldata != []:
            bt.send_move(ldata)
        else:
            print "No direction"
    else:
        print "Signal lost"
