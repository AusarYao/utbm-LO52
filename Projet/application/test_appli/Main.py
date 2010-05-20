# -*- coding:Utf-8 -*-

import Game as G
import Bluetooth as BT

#Exploration(0) or guidage(1)
MODE = 0 

#Initialisation de la connection et de la partie
bt = BT.BT()
game = G.Game(3,3)

#Affichage et connection
bt.connect()
game.m.printMap()

#mode exploration
if MODE == 0:
	if (bt.isconnected):
		data = bt.recept()
		game.update(data)
		bt.ack()
	else:
		print "Signal lost"

if MODE == 1:
	if (bt.isconnected):
		ldata = game.findDir()
		if ldata != []:
			bt.sendMove(ldata)
		else:
			print "No direction"
	else:
		print "Signal lost"
