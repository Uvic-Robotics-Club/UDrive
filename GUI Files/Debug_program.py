# -*- coding: utf-8 -*-
"""
Created on Sun Jul 08 19:12:27 2018

@author: joell
"""
from g_settings import *

class DebugScreen(QtGui.QWidget):
    def __init__(self):
        '''
        This is the Debug screen that the user interacts with.

        '''
        super(self.__class__, self).__init__()
        uic.loadUi('Debug_Window.ui', self)
        self.Max_length = 500
        print "starting up the debug screen"

    def limit_output(self, additional_text=""):
        '''
        This is where the amount of text in the debug window is limited and addes a new line
        '''
        plain_text = self.debug_output_textbox.toPlainText()
        plain_text += additional_text+"\n"
        return plain_text[-self.Max_length:]

    def debug_output(self, what_to_display):
        '''
        This essentially limits the size, adds a newline, and moves the cursor to the end
        '''
        print what_to_display
        what_to_display = self.limit_output(what_to_display)
        self.debug_output_textbox.setPlainText(what_to_display)
        self.debug_output_textbox.moveCursor(QtGui.QTextCursor.End)
