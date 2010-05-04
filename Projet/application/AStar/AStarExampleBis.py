#/usr/bin/env python

from time import time

import AStarBis
import Matrice

class AStarExample:
    def __init__(self,w,h):
        self.m = Matrice.MapData(w,h)
        self.startpoint = [1,1]
        self.endpoint = [w-2, h-2]

        self.m.setRobot(self.startpoint[0],self.startpoint[1],1)
        self.m.setFlag(self.endpoint[0],self.endpoint[1],1)

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
        #print self.m.mapdata
        self._findPath()
        print self.pathlines

if __name__ == '__main__':
    a = AStarExample(5,5)
    a.main()
