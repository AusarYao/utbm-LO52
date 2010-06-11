# -*- coding:Utf-8 -*-

import MapData as M
import Robot as R
import Flag as F
from time import time

class Game(object):
    """
    Contains map, robot, flags,...
    """
    def __init__(self, w, h, x=0, y=0, dir=1):
        """Initialize the game"""
        self.m = M.MapData(w,h)
        self.r = R.Robot(x,y,dir)
        self.f = F.Flag()

    def update_explore(self, data):
        """update data, robot and flags"""
        self.r.update2(data[1],data[2],data[3])
        if data[0] == 2: #Add Wall
            self.m.add_wall(data[1],data[2],data[3])
        elif data[0] == 3: #Add Flag
            self.f.add_flag(data[1],data[2])

    def update_guiding(self, x, y):
        """update robot and if flags are token"""
        self.r.update(x,y)
        if self.f.search_flag(x,y):
            if not self.f.mapdata[(x,y)]:
                self.f.mapdata[(x,y)] = True

    def shortest_path(self):
        """find the shortest path and nearest flag"""
        Lm = []
        first = True
        Lf = self.f.list_flag_no_taken()
        print "List flag: ",Lf
        for i in range(len(Lf)):
            L = self.m.find_path(self.r.x,self.r.y,Lf[i][0],Lf[i][1])
            print "Path n°",i,": ",L
            if first:
                Lm = L
                first = False
            elif len(L) < len(Lm):
                Lm = L
        if Lm == []:
            print "No path found"
        return Lm

    def find_next_move(self):
        """find the next move to the robot"""
        Lr = ()
        st = time()
        L = self.shortest_path()
        et = time()

        if L == []:
            return Lr
        else:
            print "Path found in %d moves and %f times" %(len(L), et-st)
            print "Shortest Path: ",L
            print "Robot: [x:%d,y:%d,dir:%d]"%(self.r.x,self.r.y,self.r.dir)
            for i in range(len(L)):
                if L[i][0] == self.r.x or L[i][1] == self.r.y:
                    Lr = L[i]
                else:
                    break
            if len(Lr)==0:
                Lr = L[0]
            return Lr


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
