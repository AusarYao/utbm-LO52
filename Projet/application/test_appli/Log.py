#!/usr/bin/env python
# -*- coding:Utf-8 -*-

import pygtk
pygtk.require("2.0")
import gtk


class MyLog(object):
    """
    Personnal class to have a log
    """
    def __init__(self, text_view, scroll):
        self.log = text_view
        self.buffer = self.log.get_buffer()
        self.buffer.set_text("")
        self.scroll = scroll
        self.scroll.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_ALWAYS)

    def add(self,string):
        if self.buffer == "":
            self.buffer.set_text(string)
            print ok
        else:
            self.iterfin = self.buffer.get_start_iter()
            self.buffer.insert(self.iterfin,string+"\n")

    def clean(self):
        self.buffer.delete(self.buffer.get_start_iter(),self.buffer.get_end_iter())
