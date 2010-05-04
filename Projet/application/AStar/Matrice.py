# -*- coding:Utf-8 -*-

from Tools import sumPow2

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
			    z += 1
			if y == size-1:
			    z += 2
			if x == size-1:
			    z += 4
			if y == 0:
			    z += 8
			self.mapdata[(x,y)] = {'flag' : 0, 'robot' : 0, 'wall' : z, 'cost' : 0}

	def getWall(self,x,y):
		return self.mapdata[(x,y)]['wall']

	def getFlag(self,x,y):
		return self.mapdata[(x,y)]['flag']

	def getRobot(self,x,y):
		return self.mapdata[(x,y)]['robot']

	def setWall(self,x,y,wall):
		self.mapdata[(x,y)]['wall'] = wall

	def setFlag(self,x,y,flag):
		self.mapdata[(x,y)]['flag'] = flag

	def setRobot(self,x,y,robot):
		self.mapdata[(x,y)]['robot'] = robot

	def isWallHere(self,x,y,value):
		liste = sumPow2(self.mapdata[(x,y)]['wall'])
		return [True for element in liste if element == value]

	
