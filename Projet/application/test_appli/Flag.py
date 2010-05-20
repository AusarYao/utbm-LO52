# -*- coding:Utf-8 -*-

class Flag(object):
    """
    Simple map with flags coordonnates
    """
    def __init__(self):
        """Initialize the map"""
        self.mapdata =[] 

    def addFlag(self,x,y):
        """Add a coordonnate flag"""
        self.mapdata.append([x,y])

    def delFlag(self,x,y):
        """Delete a coordonnate flag"""
        i = self.mapdata.index([x,y])
        del self.mapdata[i]
