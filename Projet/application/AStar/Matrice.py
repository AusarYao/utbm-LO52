# -*- coding:Utf-8 -*-

from Tools import *

class MapData:

    def __init__(self,w,h):
        #Function who allow to create a matrice n x n with a parameter
        self.mapdata = dict()
        self.mapw = w
        self.maph = h
        size = self.mapw*self.maph
        for x in xrange(self.mapw):
            for y in xrange(self.maph):
        	#Wall initialisation
        	z = 0
        	if x == 0:
        	    z += 8
        	if y == self.maph-1:
        	    z += 4
        	if x == self.mapw-1:
        	    z += 2
        	if y == 0:
        	    z += 1
        	self.mapdata[(x,y)] = {'wall' : z}

    def addWall(self,x,y,value):
        """Add the wall in the adjacent case"""
        Li = [1,2,4,8]
        if Search_in_liste(Li,value): #If value is multiple of 2
            L = Sum_pow2(self.mapdata[(x,y)]['wall'])

            if not Search_in_liste(L,value):        
                self.mapdata[(x,y)]['wall'] += value
                
                if value == 1:
                    if y!=0:
                        L = Sum_pow2(self.mapdata[(x,y-1)]['wall'])
                        if not Search_in_liste(L,4):   
                            self.mapdata[(x,y-1)]['wall'] += 4
                if value == 2:
                    if x != self.mapw-1:
                        L = Sum_pow2(self.mapdata[(x+1,y)]['wall'])
                        if not Search_in_liste(L,8):   
                            self.mapdata[(x+1,y)]['wall'] += 8
                if value == 4:
                    if y != self.maph-1:
                        L = Sum_pow2(self.mapdata[(x,y+1)]['wall'])
                        if not Search_in_liste(L,1):   
                            self.mapdata[(x,y+1)]['wall'] += 1
                if value == 8:
                    if x != 0:
                        L = Sum_pow2(self.mapdata[(x-1,y)]['wall'])
                        if not Search_in_liste(L,2):   
                            self.mapdata[(x-1,y)]['wall'] += 2
        else:
            print "Can't add"

	def isWallHere(self,x,y,value):
		liste = Sum_pow2(self.mapdata[(x,y)]['wall'])
		return [True for element in liste if element == value]

    def printMap(self):
        for x in range(self.mapw):
            for y in range(self.maph):
                print "(",x,",",y,") ",self.mapdata[(x,y)]

	
