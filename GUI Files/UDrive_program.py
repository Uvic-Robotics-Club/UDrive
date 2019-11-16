# -*- coding: utf-8 -*-
"""
Created on Sun Jul 08 18:57:32 2018

@author: joell
"""
from g_settings import *


class UDrive():
    '''
    This class is the commands that you can send to the UDrive motor driver

    '''

    def __init__(self, ser=None):
        from g_settings import debug_window
        global debug_window
        '''
        motor information can be found [here](https://www.robotshop.com/ca/en/12v-313rpm-4166oz-in-hd-premium-planetary-gearmotor-encoder.html)

        '''
        self.value_1 = 0
        self.value_2 = 0
        self.mode_1 = 0
        self.mode_2 = 0
        self.modes = ["M", "T", "V"]
        self.pid_values_m1 = [1, 10, 0] #kp, ki, kd
        self.pid_values_m2 = [1, 10, 0] #kp, ki, kd
        self.wheel_diam_m1 = 0.07  # 7 cm
        self.wheel_diam_m2 = 0.07  # 7 cm
        self.ticks_per_rev = 1288.848
        self.data_to_send = 0
        self.uC = ser
        self.debug_print = True
        self.reverse_1 = False
        self.reverse_2 = False
        self.send_enable = False

        self.arduino_input = ""
        self.write_flag = False
        print "starting the uDriver"

    def send_string(self, what_to_send):
        global debug_window
        if("~" is what_to_send):
            if(self.uC is not None and self.send_enable):
                self.uC.write(what_to_send)
        else:
            self.arduino_input = what_to_send
            self.write_flag = True
            self.send()

        #debug_window.debug_output("sent: |{}|".format(what_to_send))

    def send(self):
        if(self.write_flag):
            self.write_data()
            self.write_flag = False
            # debug_window.debug_output("sending: |{}|".format(arduino_input))
            # print colored("sending: |{}|".format(local_send), color="green")

    def stop(self):
        self.send_enable = True
        self.arduino_input = "M10,;M20, "
        self.write_data()

    def update(self):
        self.send()
        return self.read_data()

    def read_data(self):
        if(self.uC is not None):
            return self.uC.readline().strip()
        return None

    def write_data(self):
        global debug_window
        if(self.uC is not None and self.send_enable):
            self.uC.write(self.arduino_input)
            print "sent: |{}|".format(self.arduino_input)
            self.arduino_input = ""

    def create_sending_string(self):
        if(self.arduino_input != ""):
            self.write_data()
        self.arduino_input = ""
        self.arduino_input = "{}1{},;{}2{}, ".format(self.modes[self.mode_1], self.value_1, self.modes[self.mode_2], self.value_2)
        self.write_flag = True

    def manual(self, value_1, value_2):
        if(self.reverse_1):
            value_1 = -value_1
        if(self.reverse_2):
            value_2 = -value_2
        self.arduino_input = "M1{},;M2{}, ".format(value_1, value_2)
        self.write_flag = True

    def velocity(self, value_1, value_2):
        if(self.reverse_1):
            value_1 = -value_1
        if(self.reverse_2):
            value_2 = -value_2
        # print "V{},{}".format(value_1, value_2)
        self.arduino_input = "V1{},;V2{}, ".format(value_1, value_2)
        self.write_flag = True

    def position(self, value_1, value_2):
        if(self.reverse_1):
            value_1 = -value_1
        if(self.reverse_2):
            value_2 = -value_2
        # print "V{},{}".format(value_1, value_2)
        self.arduino_input = "T1{},;T2{}, ".format(value_1, value_2)
        self.write_flag = True

    def send_pid(self):
        self.arduino_input = "P1{},{},{};P2{},{},{} ".format(
                                            self.pid_values_m1[0],
                                            self.pid_values_m1[1],
                                            self.pid_values_m1[2],
                                            self.pid_values_m2[0],
                                            self.pid_values_m2[1],
                                            self.pid_values_m2[2]
                                            )
        self.write_flag = True



