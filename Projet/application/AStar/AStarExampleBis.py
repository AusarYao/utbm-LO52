# -*- coding:Utf-8 -*-

#/usr/bin/env python

from time import time

import Matrice
import AStarBis

class AStarExample:
    def __init__(self,w,h):
        self.m = Matrice.MapData(w,h)
		
		#CONFIGURATION
		
		#Localisation des points de départ
        self.startpoint = [0,0]
        self.endpoint = [w-3, h-2]
        
		#Creation de mur fictif
        self.m.addWall(0,0,4)
        self.m.addWall(1,0,2)
        self.m.addWall(1,1,2)
        self.m.addWall(1,1,4)
        self.m.addWall(0,2,2)
        self.m.addWall(1,3,2)
        self.m.addWall(2,2,2)
        self.m.addWall(2,2,4)
        self.m.addWall(3,1,8)
        self.m.addWall(1,4,2)

    def _findPath(self):
        astar = AStarBis.AStar(AStarBis.MapHandler(self.m.mapdata, self.m.mapw, self.m.maph))
        start = AStarBis.Location(self.startpoint[0], self.startpoint[1])
        end = AStarBis.Location(self.endpoint[0], self.endpoint[1])

        st = time()
        path = astar.findPath(start,end)
        et = time()

        if not path:
            print "No path found !"
        else:
            print "Path found in %d moves and %f times" % (len(path.nodes), et-st)
            self.pathlines = []
            self.pathlines.append((start.x,start.y))
            for n in path.nodes:
                self.pathlines.append((n.location.x, n.location.y))
            #self.pathlines.append((end.x,end.y))

    def main(self):
        self.m.printMap()
        self._findPath()
        print self.pathlines

if __name__ == '__main__':
    a = AStarExample(5,5)
    a.main()
