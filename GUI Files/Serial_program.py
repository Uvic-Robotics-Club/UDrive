# -*- coding: utf-8 -*-
"""
Created on Sun Jul 08 19:22:35 2018

@author: joell
"""
from g_settings import *

class SerialScreen(QtGui.QWidget):
    def __init__(self):
        global mc
        '''
        This is the serial selector screen that the user interacts with.

        '''
        super(self.__class__, self).__init__()
        uic.loadUi('Serial_Port_Selector.ui', self)
        self.getComms()
        self.pushButton_2.clicked.connect(self.ok_callback)
        self.pushButton.clicked.connect(self.time_to_close)
        print  "starting up the serial screen"

    def getComms(self):
        # print "Getting all of the available ports"
        self.ports = list(list_ports.comports())
        for (port, name, PID) in self.ports:
            # print "Testing {} which is port: {}".format(name, port)
            self.comboBox.addItem("{} -> {}".format(name, port))

    def ok_callback(self):
        try:
            from g_settings import mc, debug_window
            print "trying to connect to {}".format(self.ports[self.comboBox.currentIndex()][0])
            ser = serial.Serial(self.ports[self.comboBox.currentIndex()][0],115200,timeout=None)
            mc.uC = ser
            debug_window.debug_output("connected to the arduino")
            self.time_to_close()
        except serial.serialutil.SerialException:
            debug_window.debug_output("Could not connect to the selected Port")

    def time_to_close(self):
        self.close()

