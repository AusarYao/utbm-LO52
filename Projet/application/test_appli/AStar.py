#/usr/bin/env python
# -*- coding:Utf-8 -*-

import Tools

class Location:
    """Represent simple map Location implementation"""
    def __init__(self,x,y):
        self.x = x
        self.y = y

    def __eq__(self, l):
        if l.x == self.x and l.y == self.y:
            return 1
        else:
            return 0

class Path:
    """Contains the path of nodes"""
    def __init__(self,nodes, totalCost):
        self.nodes = nodes;
        self.totalCost = totalCost;

class Node:
    """Main element to compute the path"""
    def __init__(self,location,mCost,lid,wall=0,parent=None):
        self.location = location # where is this node located
        self.mCost = mCost # total move cost to reach this node
        self.parent = parent # parent node
        self.score = 0 # calculated score for this node
        self.lid = lid # set the location id - unique for each location in the map
        self.wall = wall

    def __eq__(self, n):
        if n.lid == self.lid:
            return 1
        else:
            return 0

class AStar:
    """To calculate the path in labyrinth"""
    def __init__(self,maphandler):
        self.mh = maphandler
                
    def _getBestOpenNode(self):
        bestNode = None        
        for n in self.on:
            if not bestNode:
                bestNode = n
            else:
                if n.score<=bestNode.score:
                    bestNode = n
        return bestNode

    def _tracePath(self,n):
        nodes = [];
        totalCost = n.mCost;
        p = n.parent;
        nodes.insert(0,n);       
        
        while 1:
            if p.parent is None: 
                break

            nodes.insert(0,p)
            p=p.parent
        
        return Path(nodes,totalCost)

    def _handleNode(self,node,end):        
        i = self.o.index(node.lid)
        self.on.pop(i)
        self.o.pop(i)
        self.c.append(node.lid)

        nodes = self.mh.getAdjacentNodes(node,end)

        for n in nodes:
            if n.location == end:
                # reached the destination
                return n
            elif n.lid in self.c:
                # already in close, skip this
                continue
            elif n.lid in self.o:
                # already in open, check if better score
                i = self.o.index(n.lid)
                on = self.on[i];
                if n.mCost<on.mCost:
                    self.on.pop(i);
                    self.o.pop(i);
                    self.on.append(n);
                    self.o.append(n.lid);
            else:
                # new node, append to open list
                self.on.append(n);                
                self.o.append(n.lid);

        return None

    def findPath(self,fromlocation, tolocation):
        self.o = []
        self.on = []
        self.c = []

        end = tolocation
        firstNode = self.mh.getNode(fromlocation)
        self.on.append(firstNode)
        self.o.append(firstNode.lid)
        nextNode = firstNode 
               
        while nextNode is not None:
            finish = self._handleNode(nextNode,end)
            if finish:                
                return self._tracePath(finish)
            nextNode=self._getBestOpenNode()
                
        return None

class MapHandler:
    """Square map implementation"""

    def __init__(self,mapdata,width,height):
        self.m = mapdata
        self.w = width
        self.h = height

    def getNode(self, location):
        """MUST BE IMPLEMENTED"""
        x = location.x
        y = location.y
        if x<0 or x>=self.w or y<0 or y>=self.h:
            return None
        w = self.m[(x,y)]['wall'] # wall
        return Node(location,0,((y*self.w)+x),w);                

    def getAdjacentNodes(self, curnode, dest):
        """MUST BE IMPLEMENTED"""        
        result = []
       
        cl = curnode.location
        dl = dest
        
        #Decompose the value of wall
        wall_decompose = Tools.Sum_pow2(self.m[(cl.x,cl.y)]['wall'])
        list_wall = [1,2,4,8]
        for i in wall_decompose:
            list_wall.remove(i)
        #Test for each value if a wall exist
        if Tools.Search_in_liste(list_wall,2):
            n = self._handleNode(cl.x+1,cl.y,curnode,dl.x,dl.y)
            result.append(n)

        if Tools.Search_in_liste(list_wall,8):
            n = self._handleNode(cl.x-1,cl.y,curnode,dl.x,dl.y)
            result.append(n)
            
        if Tools.Search_in_liste(list_wall,4):
            n = self._handleNode(cl.x,cl.y+1,curnode,dl.x,dl.y)
            result.append(n)
            
        if Tools.Search_in_liste(list_wall,1):
            n = self._handleNode(cl.x,cl.y-1,curnode,dl.x,dl.y)
            result.append(n)

        return result

    def _handleNode(self,x,y,fromnode,destx,desty):
        n = self.getNode(Location(x,y))
        dx = max(x,destx) - min(x,destx)
        dy = max(y,desty) - min(y,desty)
        emCost = dx+dy # Distance cost
        n.mCost += fromnode.mCost # cost of current node
        n.score = n.mCost+emCost
        n.parent=fromnode
        return n  
