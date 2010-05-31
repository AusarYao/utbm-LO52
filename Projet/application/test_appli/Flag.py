# -*- coding:Utf-8 -*-

class Flag(object):
    """
    Simple map with flags coordonnates
    """
    def __init__(self):
        """Initialize the map"""
        self.mapdata = dict()

    def add_flag(self, x, y):
        """Add a coordonnate flag"""
        self.mapdata[(x, y)] = True

    def del_flag(self, x, y):
        """Delete a coordonnate flag"""
        i = self.mapdata.index([x, y])
        del self.mapdata[i]

    def init_flag(self):
        """Discount flags to no-taken"""
        L = self.mapdata.keys()
        for x in range(len(L)):
            self.mapdata[L[x]] = False

    def list_flag(self):
        """List the coordonnates of the case where the flag are not taken"""
        Lr = []
        L = self.mapdata.keys()
        for x in range(len(L)):
            if self.mapdata[L[x]] == False:
                Lr.append(L[x])
        return Lr

    def search_flag(self, x, y):
        """Search if a flag exist at this coordonnates"""
        L = self.mapdata.keys()
        for i in range(len(L)):
            if L[i] == (x,y):
                return True
        return False
