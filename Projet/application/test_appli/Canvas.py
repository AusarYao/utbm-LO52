#!/usr/bin/env python
# -*- coding:Utf-8 -*-

import pygtk
pygtk.require("2.0")
import gtk
import Game as G

SIZE_CASE = 32

class Coord(object):
    """
    Basic class who symbolise a coordinate
    """
    def __init__(self,x=0,y=0):
        self.x = x*SIZE_CASE
        self.y = y*SIZE_CASE


class Area(object):
    """
    Basic class who symbolise an area
    """
    def __init__(self,c1,c2):
        self.c1 = c1
        self.c2 = c2

    def getX(self):
        if self.c1.x > self.c2.x:
            return self.c2.x
        return self.c1.x

    def getY(self):
        if self.c1.y > self.c2.y:
            return self.c2.y
        return self.c1.y

    def getHeight(self):
        return abs(self.c1.y - self.c2.y)

    def getWidth(self):
        return abs(self.c1.y - self.c2.y)


class Color(object):
    """
    Basic class who symbolise a color
    """
    def __init__(self,red,green,blue,acc):
        self.red = red*acc
        self.green = green*acc
        self.blue = blue*acc


class Canvas(object):
    """
    Basic class who symbolise a drawing area
        - Draw rectangle
    """
    def __init__(self,canvas):
        self.canvas = canvas
        self.style = self.canvas.get_style()
        self.gc = self.style.fg_gc[gtk.STATE_NORMAL]
        self.drawable = self.canvas.window
        self.gc.set_clip_origin(10,10)
        self.gc.set_ts_origin(10,10)

    def setColor(self,string):
        color = self.drawable.get_colormap().alloc_color(string)#(red*mult, green*mult, blue*mult)
        self.gc.set_foreground(color)

    def drawRectangle(self,area,color=None,fill=True):
        if not color is None:
            self.setColor(color)
        self.drawable.draw_rectangle(self.gc,fill, area.getX(),area.getY(),area.getWidth(),area.getHeight())
        self.setColor("black")

    def drawLine(self, source, dest, size=1,  color=None):
        if not color is None:
            self.setColor(color)
        self.gc.line_width = size
        self.drawable.draw_line(self.gc,source.x, source.y, dest.x, dest.y)
        self.gc.line_width = 1
        self.setColor("black")

    def update(self):
        pass #a definir

class MyCanvas(object):
    """
    Personnal class
        - Draw a rectangle
    """
    def __init__(self,canvas,game):
        self.draw = Canvas(canvas)
        self.game = game

    def drawMap(self):
        self.game.m
        x = 0
        y = 0
        for x in xrange(self.game.m.mapw):
            for y in xrange(self.game.m.maph):
                #Print the map with robot and flags
                if self.game.r.x == x and self.game.r.y == y:
                    self.draw.drawRectangle(Area(Coord(x,y),Coord(x+1,y+1)), "red",True)
                elif self.game.f.search_flag(x,y):
                    if self.game.f.mapdata[(x,y)]:
                        self.draw.drawRectangle(Area(Coord(x,y),Coord(x+1,y+1)), "green",True)
                    else:
                        self.draw.drawRectangle(Area(Coord(x,y),Coord(x+1,y+1)), "orange",True)
                else:
                    self.draw.drawRectangle(Area(Coord(x,y),Coord(x+1,y+1)), "white",True)

                #Print the wall
                if self.game.m.is_wall_here(x,y,1): #Wall on top
                    self.draw.drawLine(Coord(x,y),Coord(x+1,y),3)
                else:
                    self.draw.drawLine(Coord(x,y),Coord(x+1,y))

                if self.game.m.is_wall_here(x,y,4): #Wall on bottom
                    self.draw.drawLine(Coord(x,y+1),Coord(x+1,y+1),3)
                else:
                    self.draw.drawLine(Coord(x,y+1),Coord(x+1,y+1))

                if self.game.m.is_wall_here(x,y,2): #Wall on right
                    self.draw.drawLine(Coord(x+1,y),Coord(x+1,y+1),3)
                else:
                    self.draw.drawLine(Coord(x+1,y),Coord(x+1,y+1))

                if self.game.m.is_wall_here(x,y,8): #Wall on left
                    self.draw.drawLine(Coord(x,y),Coord(x,y+1),3)
                else:
                    self.draw.drawLine(Coord(x,y),Coord(x,y+1))

    def update(self):
        self.drawMap()
