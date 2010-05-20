# -*- coding:Utf-8 -*-

import MapData as M
import Robot as R
import Flag as F
from time import time

class Game(object):
    """
    Contains map, robot, flags,...
    """
    def __init__(self,w,h,x=0,y=0,dir=1):
        """Initialize the game"""
        self.m = M.MapData(w,h)
        self.r = R.Robot(x,y,dir)
        self.f = F.Flag()

    def update(self,data):
        """update data, robot and flags"""
        print data
        l = data.split(":")
        l = [int(l[x]) for x in xrange(len(l))]
        self.r.update(l[0],l[1],l[2])
        self.m.add_wall(l[0],l[1],l[3])
        if l[4] != 0:
            self.f.add_flag(l[0],l[1])
        self.m.print_map()

    def shortest_path(self):
        """find the shortest path and nearest flag"""
        Lm = []
        first = True
        for x in range(len(self.f.mapdata)):
            L = self.m.find_path(self.r.x,self.r.y,self.f.mapdata[x][0],self.f.mapdata[x][1])
            print L
            if first:
                Lm = L
            elif len(L) < len(Lm):
                Lm = L
        return Lm

    def find_dir(self):
        """find the direction and the move to the robot"""
        Lr = []
        st = time()
        L = self.shortest_path()
        et = time()
        if L==[]:
            print "No path found"
        else:
            print "Path found in %d moves and %f times" % (len(L), et-st)
            print L
            print "[x:%d,y:%d,dir:%d]"%(self.r.x,self.r.y,self.r.dir)
            if (self.r.x - 1) == L[0][0]: #case en haut
                if self.r.dir==2:
                    Lr.append(1)
                elif self.r.dir==8:
                    Lr.append(2)
                else:
                    Lr.append(1)
                    Lr.append(1)
                Lr.append(0)
            if (self.r.x + 1) == L[0][0]: #case en bas
                if self.r.dir==2:
                    Lr.append(2)
                elif self.r.dir==8:
                    Lr.append(1)
                else:
                    Lr.append(1)
                    Lr.append(1)
                Lr.append(0)
            if (self.r.y + 1) == L[0][1]: #case à droite
                if self.r.dir==1:
                    Lr.append(2)
                elif self.r.dir==4:
                    Lr.append(1)
                else:
                    Lr.append(1)
                    Lr.append(1)
                Lr.append(0)
            if (self.r.y - 1) == L[0][1]: #case à gauche
                if self.r.dir==1:
                    Lr.append(1)
                elif self.r.dir==4:
                    Lr.append(2)
                else:
                    Lr.append(1)
                    Lr.append(1)
                Lr.append(0)

        return Lr
