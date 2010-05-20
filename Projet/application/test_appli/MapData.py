# -*- coding:Utf-8 -*-

import Tools as T
import AStar as AS

class MapData(object):
    """
        
    """
    def __init__(self,w,h):
        """Initialize the map"""
        self.mapdata = dict()
        self.mapw = w
        self.maph = h
        for x in xrange(self.mapw):
            for y in xrange(self.maph):
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
        if T.Search_in_liste(Li,value): #If value is multiple of 2
            L = T.Sum_pow2(self.mapdata[(x,y)]['wall'])

            if not T.Search_in_liste(L,value):
                self.mapdata[(x,y)]['wall'] += value
                print "Wall added (%d,%d):%d"%(x,y,value)
                if value == 1:
                    if y!=0:
                        L = T.Sum_pow2(self.mapdata[(x,y-1)]['wall'])
                        if not T.Search_in_liste(L,4):   
                            self.mapdata[(x,y-1)]['wall'] += 4
                if value == 2:
                    if x != self.mapw-1:
                        L = T.Sum_pow2(self.mapdata[(x+1,y)]['wall'])
                        if not T.Search_in_liste(L,8):   
                            self.mapdata[(x+1,y)]['wall'] += 8
                if value == 4:
                    if y != self.maph-1:
                        L = T.Sum_pow2(self.mapdata[(x,y+1)]['wall'])
                        if not T.Search_in_liste(L,1):   
                            self.mapdata[(x,y+1)]['wall'] += 1
                if value == 8:
                    if x != 0:
                        L = T.Sum_pow2(self.mapdata[(x-1,y)]['wall'])
                        if not T.Search_in_liste(L,2):   
                            self.mapdata[(x-1,y)]['wall'] += 2
            else:
                print "Can't add (%d,%d):%d"%(x,y,value)
        else:
            print "Error value (%d)"%(value)

    def isWallHere(self,x,y,value):
        """Check if a wall exist"""
        liste = T.Sum_pow2(self.mapdata[(x,y)]['wall'])
        return [True for element in liste if element == value]

    def printMap(self):
        """Print map"""
        for x in range(self.mapw):
            for y in range(self.maph):
                print "(",x,",",y,") ",self.mapdata[(x,y)]

    def findPath(self,start_x,start_y,end_x,end_y):
        astar = AS.AStar(AS.MapHandler(self.mapdata,self.mapw,self.maph))
        start = AS.Location(start_x,start_y)
        end = AS.Location(end_x,end_y)

        path = astar.findPath(start,end)

        if not path:
            print "No path found !"
        else:
            self.pathlines = []
            for n in path.nodes:
                self.pathlines.append((n.location.x, n.location.y))

        return self.pathlines
