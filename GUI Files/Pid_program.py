# -*- coding: utf-8 -*-
"""
Created on Tue Jul 17 13:38:10 2018

@author: joell
"""

from g_settings import *

class PidScreen(QtGui.QWidget):
    def __init__(self): #THIS IS SUPER IMPORTANT
        super(self.__class__, self).__init__() #THIS IS SUPER IMPORTANT
        uic.loadUi('pid.ui', self) #THIS IS WHERE YOU LOAD THE .UI FILE
        self.horizontalSlider_kp.valueChanged.connect(self.slider_changed_kp)
        self.horizontalSlider_ki.valueChanged.connect(self.slider_changed_ki)
        self.horizontalSlider_kd.valueChanged.connect(self.slider_changed_kd)
        self.pushButton.clicked.connect(self.send_callback)

    def my_map(self, x, in_min, in_max, out_min, out_max):
        return (x - in_min)*(out_max-out_min)/(in_max-in_min)+out_min

    def slider_changed_kp(self):
        min_value = self.doubleSpinBox.value()
        max_value = self.doubleSpinBox_2.value()
        value = float(self.horizontalSlider_kp.value())/100

        self.spinner_kp.setValue(self.my_map(value, 0.0, 1.0, min_value, max_value))
        self.send_value(self.checkBox.isChecked())

    def slider_changed_ki(self):
        min_value = self.doubleSpinBox_3.value()
        max_value = self.doubleSpinBox_5.value()
        value = float(self.horizontalSlider_ki.value())/100

        self.spinner_ki.setValue(self.my_map(value, 0.0, 1.0, min_value, max_value))
        self.send_value(self.checkBox.isChecked())

    def slider_changed_kd(self):
        min_value = self.doubleSpinBox_4.value()
        max_value = self.doubleSpinBox_6.value()
        value = float(self.horizontalSlider_kd.value())/100

        self.spinner_kd.setValue(self.my_map(value, 0.0, 1.0, min_value, max_value))
        self.send_value(self.checkBox.isChecked())

    def send_callback(self):
        self.send_value(True)

    def send_value(self, button = True):
        if(button==True):
            from g_settings import mc
            mc.pid_values_m1[0] = self.spinner_kp.value()
            mc.pid_values_m1[1] = self.spinner_ki.value()
            mc.pid_values_m1[2] = self.spinner_kd.value()
            mc.send_pid()
