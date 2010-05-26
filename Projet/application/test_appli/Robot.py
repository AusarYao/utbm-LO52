# -*- coding:Utf-8 -*-

class Robot(object):
    """
    Robot coordonnates
    """

    def __init__(self, x=0, y=0, dir=1):
        self.x = x
        self.y = y
        self.dir = dir

    def update(self, x, y, dir):
        self.x = x
        self.y = y
        self.dir = dir

    def update(self, x, y):
        self.x = x
        self.y = y
