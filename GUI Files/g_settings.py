# -*- coding: utf-8 -*-
"""
Created on Sun Jul 08 18:24:12 2018

@author: joell
"""
import time
import numpy as np
from termcolor import colored

from PyQt4 import QtCore, QtGui, uic  # Import the PyQt4 module we'll need
from PyQt4.QtCore import *

import serial
import serial.tools.list_ports as list_ports

from UDrive_program import UDrive
from Debug_program import DebugScreen
from Serial_program import SerialScreen
from Motor_Driver_Settings import MCD_Settings
from Pid_program import PidScreen

base_time = time.time()


m1_data = {"setpoint": np.array([0.0, 0.0]), "output": np.array([0.0, 0.0]), "input": np.array([0.0, 0.0])}
m2_data = {"setpoint": np.array([0.0, 0.0]), "output": np.array([0.0, 0.0]), "input": np.array([0.0, 0.0])}

m1_show_lines = {"setpoint": True, "output": True, "input": True}
m2_show_lines = {"setpoint": False, "output": False, "input": False}

m1_linetypes = {"setpoint": "--", "output": "-", "input": "-"}
m2_linetypes = {"setpoint": "--", "output": "-", "input": "-"}

m1_colours = {"setpoint": "r", "output": "g", "input": "b"}
m2_colours = {"setpoint": "r", "output": "b", "input": "g"}


xdata = np.array([base_time, base_time])
run_flag = True
write_flag = False
data_size = 1000
arduino_input = ""

debug_window = DebugScreen() # this has to be the first window that is created

ser = None
mc = UDrive(ser)


serial_window = SerialScreen() # mc has to be created before this
mcd_window = MCD_Settings()
pid_window = PidScreen()

print "starting up the gobal settings"

