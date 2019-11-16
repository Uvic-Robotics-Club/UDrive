# -*- coding: utf-8 -*-
"""
Created on Mon Jun 04 17:55:33 2018

@author: joell

Lines 214-270 are all the code for connecting the GUI to the code
"""


from PyQt4 import QtCore, QtGui, uic  # Import the PyQt4 module we'll need
from PyQt4.QtCore import *
import sys  # We need sys so that we can pass argv to QApplication
app = QtGui.QApplication(sys.argv) # this is important needs to be before the g_settings
from g_settings import * # all the global variables


class Graph_settings(QtGui.QWidget):
    def __init__(self):
        super(self.__class__, self).__init__()  # THIS IS SUPER IMPORTANT
        uic.loadUi('graph_settings.ui', self)  # THIS IS WHERE YOU LOAD THE .UI FILE
        self.pushButton.clicked.connect(self.set_graph_callback)
        self.pushButton_2.clicked.connect(self.reset_menu)
        self.colours = ['b', 'g', 'r', 'c', 'm', 'y', 'k']
        self.lines = ['-', '--', '-.', ':']

    def reset_menu(self):
        # 2 is checked and 0 is not checked and 1 is partial check
        self.m1_input_show.setCheckState(2)
        self.m1_setpoint_show.setCheckState(2)
        self.m1_output_show.setCheckState(2)

        self.m2_input_show.setCheckState(0)
        self.m2_setpoint_show.setCheckState(0)
        self.m2_output_show.setCheckState(0)

        self.m1_input_color.setCurrentIndex(0)
        self.m1_setpoint_color.setCurrentIndex(2)
        self.m1_output_color.setCurrentIndex(1)

        self.m2_input_color.setCurrentIndex(0)
        self.m2_setpoint_color.setCurrentIndex(0)
        self.m2_output_color.setCurrentIndex(0)

        self.m1_input_linetype.setCurrentIndex(0)
        self.m1_setpoint_linetype.setCurrentIndex(1)
        self.m1_output_linetype.setCurrentIndex(0)

        self.m2_input_linetype.setCurrentIndex(0)
        self.m2_setpoint_linetype.setCurrentIndex(0)
        self.m2_output_linetype.setCurrentIndex(0)

    def set_graph_callback(self):
        global m1_colours, m2_colours
        global m1_show_lines, m2_show_lines
        global m1_linetypes, m2_linetypes

        m1_show_lines['setpoint'] = self.m1_setpoint_show.isChecked()
        m1_show_lines['output'] = self.m1_output_show.isChecked()
        m1_show_lines['input'] = self.m1_input_show.isChecked()

        m2_show_lines['setpoint'] = self.m2_setpoint_show.isChecked()
        m2_show_lines['output'] = self.m2_output_show.isChecked()
        m2_show_lines['input'] = self.m2_input_show.isChecked()

        m1_colours['setpoint'] = self.colours[self.m1_setpoint_color.currentIndex()]
        m1_colours['output'] = self.colours[self.m1_output_color.currentIndex()]
        m1_colours['input'] = self.colours[self.m1_input_color.currentIndex()]

        m2_colours['setpoint'] = self.colours[self.m2_setpoint_color.currentIndex()]
        m2_colours['output'] = self.colours[self.m2_output_color.currentIndex()]
        m2_colours['input'] = self.colours[self.m2_input_color.currentIndex()]

        m1_linetypes['setpoint'] = self.lines[self.m1_setpoint_linetype.currentIndex()]
        m1_linetypes['output'] = self.lines[self.m1_output_linetype.currentIndex()]
        m1_linetypes['input'] = self.lines[self.m1_input_linetype.currentIndex()]

        m2_linetypes['setpoint'] = self.lines[self.m2_setpoint_linetype.currentIndex()]
        m2_linetypes['output'] = self.lines[self.m2_output_linetype.currentIndex()]
        m2_linetypes['input'] = self.lines[self.m2_input_linetype.currentIndex()]



class MainScreen(QtGui.QMainWindow):
    def __init__(self): #THIS IS SUPER IMPORTANT
        global debug_window
        from usbWorker import Worker
        '''
        This is the main screen that the user interacts with.

        '''
        super(self.__class__, self).__init__() #THIS IS SUPER IMPORTANT
        uic.loadUi('Main_Window.ui', self) #THIS IS WHERE YOU LOAD THE .UI FILE
        timer = QtCore.QTimer(self)
        timer.timeout.connect(self.displayGraph)
        timer.start(500)
        self.threadpool = QtCore.QThreadPool()

        self.timer2 = QtCore.QTimer(self)
        self.timer2.timeout.connect(self.get_new_data)
        self.timer2.start(200)

        self.worker = Worker()
        self.worker.signals.result.connect(self.print_output)
        self.threadpool.start(self.worker)
        self.toggle_powerbar_callback()

        self.graph_settings = Graph_settings()

        # File Menus
        conn_action = QtGui.QAction("&Connect To UDrive", self)
        conn_action.setShortcut("Ctrl+O")
        conn_action.setStatusTip('Connect to the UDrive system')
        conn_action.triggered.connect(self.connect_to_mc_callback)

        main_menu = self.menuBar()
        file_menu = main_menu.addMenu('&File')
        file_menu.addAction(conn_action)

        # Config Menus
        power_action = QtGui.QAction("&Toggle Power Bars", self)
        power_action.setStatusTip("Enable/Disable Power Bars")
        power_action.triggered.connect(self.toggle_powerbar_callback)

        debug_action = QtGui.QAction("&Debug", self)
        debug_action.setStatusTip("Show the Debug window")
        debug_action.triggered.connect(self.show_debug_callback)

        mcd_action = QtGui.QAction("&Motor Driver Settings", self)
        mcd_action.setStatusTip("Configure the DRV8704 Settings")
        mcd_action.triggered.connect(self.show_mcd_callback)

        pid_action = QtGui.QAction("&PID adjustment settings", self)
        pid_action.triggered.connect(self.show_pid_callback)

        reset_action = QtGui.QAction("&Reset", self)
        reset_action.triggered.connect(self.reset_callback)

        sleep_action = QtGui.QAction("Sleep", self)
        sleep_action.triggered.connect(self.sleep_callback)

        stop_action = QtGui.QAction("&Stop motors", self)
        stop_action.triggered.connect(self.stop_callback)
        stop_action.setShortcut("Space")

        graph_action = QtGui.QAction("Graph Settings", self)
        graph_action.triggered.connect(self.graph_settings_callback)



        config_menu = main_menu.addMenu("&Config")
        config_menu.addAction(graph_action)
        config_menu.addAction(power_action)
        config_menu.addAction(debug_action)
        config_menu.addAction(mcd_action)
        config_menu.addAction(pid_action)
        config_menu.addAction(sleep_action)
        config_menu.addAction(reset_action)
        config_menu.addAction(stop_action)

        # start of all of the connected objects

        # status

        # Commands
        self.start_button.clicked.connect(self.start_pause_callback)
        self.stop_button.clicked.connect(self.stop_callback)
        self.reset_button.clicked.connect(self.reset_callback)

        # Main Command
        self.motor_1_spinner.valueChanged.connect(self.motor_1_callback)
        self.motor_2_spinner.valueChanged.connect(self.motor_2_callback)
        self.comboBox.currentIndexChanged.connect(self.combobox_callback)
        #self.motor_1_fwd_rev.clicked.connect(self.motor_1_fwd_rev_callback)
        #self.motor_2_fwd_rev.clicked.connect(self.motor_2_fwd_rev_callback)
        debug_window.debug_output("starting up the main window")

        self.pi = 3.141592653589793

    def closeEvent(self, event):
        global run_flag
        run_flag = False
        debug_window.close()
        serial_window.close()
        self.close()
        event.accept()

    def graph_settings_callback(self):
        self.graph_settings.show()

    def show_pid_callback(self):
        debug_window.debug_output("showing the pid window")
        pid_window.show()

    def show_mcd_callback(self):
        debug_window.debug_output("showing the mcd window")
        mcd_window.show()

    def show_debug_callback(self):
        debug_window.debug_output("showing the debug window")
        debug_window.show()

    def connect_to_mc_callback(self):
        debug_window.debug_output("connection to micro controller callback")
        serial_window.show()
        # print "Connection to mC callback"

    def toggle_powerbar_callback(self):
        if(self.motor_1_power_bar.isHidden()):
            debug_window.debug_output("Enabling the power bars")
            self.motor_1_power_bar.show()
            self.motor_2_power_bar.show()
        else:
            debug_window.debug_output("Disabling the power bars")
            self.motor_1_power_bar.hide()
            self.motor_2_power_bar.hide()

    def motor_1_fwd_rev_callback(self):
        ''' what is called when you click the reverse button for motor 1 '''
        mc.reverse_1 = not mc.reverse_1
        self.mode_value_callbacks()

    def motor_2_fwd_rev_callback(self):
        ''' what is called when you click the reverse button for motor 2 '''
        mc.reverse_2 = not mc.reverse_2
        self.mode_value_callbacks()

    def motor_1_callback(self):
        ''' what is called when you change the dial or spinner for motor 1'''
        mc.value_1 = self.motor_1_spinner.value()
        if(self.motor_1_fwd_rev.isChecked()):
            mc.value_1 = - mc.value_1
        if(self.motor_link.isChecked()):
            self.motor_2_spinner.setValue(self.motor_1_spinner.value())
            mc.value_2 = mc.value_1
        self.mode_value_callbacks()

    def motor_2_callback(self):
        ''' what is called when you change the dial or spinner for motor 2 '''
        mc.value_2 = self.motor_2_spinner.value()
        if(self.motor_2_fwd_rev.isChecked()):
            mc.value_2 = - mc.value_2
        self.mode_value_callbacks()

    def mode_value_callbacks(self):
        '''
        This is called every time the dials or spinners are changed.

        This is because the meaning of the information changes when the mode changes
        '''
        mc.mode_1 = self.comboBox.currentIndex()
        mc.mode_2 = self.comboBox_2.currentIndex()
        mc.create_sending_string()


    def combobox_callback(self, changed_index):
        '''
        This is called every time you make a CHANGE to the mode dropdown box.

        If you go from one mode to the same mode it will not go into this funciton.
        '''
        debug_window.debug_output("mode changed to {}".format(self.comboBox.currentText()))
        if(changed_index == 0):
            # this is duty cycle
            self.dial_label.setText("Duty Cycle")
        elif(changed_index == 1):
            # this is position
            self.dial_label.setText("Distance in M")
        elif(changed_index == 2):
            # this is Velocity
            self.dial_label.setText("Speed in RPM")


    def start_pause_callback(self):
        '''
        This is called every time you click the start button

        If start is pressed then it changes to pause
        If pause is pressed the it changes to start
        '''
        if(self.start_button.text() == "Start" ):
            debug_window.debug_output("Start Button Pressed")
            mc.send_enable = True
            self.start_button.setText("Pause")
            self.start_button.setToolTip("Stop Streaming data to the micro controller")
            self.mode_value_callbacks()
        elif(self.start_button.text() == "Pause" ):
            debug_window.debug_output("Pause Button Pressed")
            mc.send_enable = False
            self.start_button.setText("Start")
            self.start_button.setToolTip("Start Streaming data to the micro controller")

    def stop_callback(self):
        ''' This is called every time the stop button is pressed '''
        debug_window.debug_output("Stop Button Pressed")
        mc.stop()
        mc.send_enable = False
        self.start_button.setText("Start")
        self.start_button.setToolTip("Start Streaming data to the micro controller")

    def get_new_data(self):
        mc.send_string("~")



    def reset_callback(self):
        debug_window.debug_output("Reset Button Pressed")
        mc.send_string("R")

    def sleep_callback(self):
        debug_window.debug_output("Sleep button pressed")
        mc.send_string("S")


    def send_velocity(self):
        value_1 = self.motor_1_spinner.value()
        value_2 = self.motor_2_spinner.value()
        mc.velocity(value_1, value_2)

    def send_manual(self):
        value_1 = self.motor_1_spinner.value()
        value_2 = self.motor_2_spinner.value()
        mc.manual(value_1, value_2)

    def send_position(self):
        value_1 = self.motor_1_spinner.value()
        value_2 = self.motor_2_spinner.value()
        # want to set the values from 0 to 1 m
        # mc.wheel_diam = 0.01  # 10 cm
        # mc.ticks_per_rev = 1296
        value_1 *= mc.ticks_per_rev*mc.wheel_diam*self.pi
        value_2 *= 0.01

        mc.position(value_1, value_2)

    def is_num(self, string):
        try:
            float(string)
            return True
        except Exception:
            return False

    def print_output(self, s):
        global m1_data, m2_data

        if "S" in s:
            print "settings: |{}|".format(s)

        try:
            split_csv = s.split(",")
            if len(split_csv) < 6:
                print "length is not 6: |{}|  {}".format(s, split_csv)
            else:
                try:
                    for i in range(6):
                        if(".-" in split_csv[i]):
                            split_csv[i] =  split_csv[i].partition(".-")[0]
                        split_csv[i] = float(split_csv[i])
                except Exception:
                    print "ERROR IN READING THE SPLIT CSV: |{}|  {}".format(s, split_csv)
                    return
                if(len(m1_data['input']) < data_size):
                    m1_data['input'] = np.append(m1_data['input'], split_csv[0])
                else:
                    m1_data['input'] = np.roll(m1_data['input'], -1)
                    m1_data['input'][-1] = split_csv[0]

                if(len(m1_data['setpoint']) < data_size):
                    m1_data['setpoint'] = np.append(m1_data['setpoint'], split_csv[1])
                else:
                    m1_data['setpoint'] = np.roll(m1_data['setpoint'], -1)
                    m1_data['setpoint'][-1] = split_csv[1]

                if(len(m1_data['output']) < data_size):
                    m1_data['output'] = np.append(m1_data['output'], split_csv[2])
                else:
                    m1_data['output'] = np.roll(m1_data['output'], -1)
                    m1_data['output'][-1] = split_csv[2]

                if(len(m2_data['input']) < data_size):
                    m2_data['input'] = np.append(m2_data['input'], split_csv[3])
                else:
                    m2_data['input'] = np.roll(m2_data['input'], -1)
                    m2_data['input'][-1] = split_csv[3]

                if(len(m2_data['setpoint']) < data_size):
                    m2_data['setpoint'] = np.append(m2_data['setpoint'], split_csv[4])
                else:
                    m2_data['setpoint'] = np.roll(m2_data['setpoint'], -1)
                    m2_data['setpoint'][-1] = split_csv[4]

                if(len(m2_data['output']) < data_size):
                    m2_data['output'] = np.append(m2_data['output'], split_csv[5])
                else:
                    m2_data['output'] = np.roll(m2_data['output'], -1)
                    m2_data['output'][-1] = split_csv[5]

        except SyntaxError:
            # print colored("-> {} -> ".format(s), "green")
            debug_window.debug_output("-> {}".format(s))
        #print colored("-> {}".format(s), "green")

    def displayGraph(self):
        global m1_data, m2_data
        global m1_show_lines, m2_show_lines
        global m1_linetypes, m2_linetypes
        global m1_colours, m2_colours

        self.mplwidget.axes.cla()
        self.mplwidget.axes.hold(True)
        ''' plot the data! '''
        for key in m1_data:
            if(m1_show_lines[key]):
                self.mplwidget.axes.plot(np.linspace(0, 10, len(m1_data[key]), endpoint=True), m1_data[key], "{}{}".format(m1_colours[key], m1_linetypes[key]), label="M1 {}".format(key))
            if(m2_show_lines[key]):
                self.mplwidget.axes.plot(np.linspace(0, 10, len(m2_data[key]), endpoint=True), m2_data[key], "{}{}".format(m2_colours[key], m2_linetypes[key]), label="M2 {}".format(key))

        self.mplwidget.axes.legend(loc= 'upper left')

        #self.mplwidget.axes.plot(np.linspace(0, 10, len(input_data), endpoint=True), input_data, 'b-',  # blue solid line
        #                         np.linspace(0, 10, len(m1_output_data), endpoint=True) , m1_output_data, 'g:',  # green dotted line
        #                         np.linspace(0, 10, len(setpoint_data), endpoint=True) , setpoint_data, 'r--'  # red dashed line
        #                         )

        self.mplwidget.axes.hold(False)
        ''' labels are nice to have '''
        self.mplwidget.axes.set_xlabel("time")
        self.mplwidget.axes.set_ylabel("arbartary scale")

        ''' redraw the graph to display the new graph '''
        self.mplwidget.draw()


try:
    global ser, mc, debug_window, serial_window, form
    debug_window.debug_output("trying to start the mc")
    # mc = UDrive(ser)

    #debug_window = DebugScreen()
    #serial_window = SerialScreen()
    form = MainScreen() #this is important

    form.show() #this is important
    app.exec_() #this is important
finally:
    if(mc.uC is not None):
        mc.uC.close()
        print colored("arduino closed", 'magenta')
    else:
        print colored("no arduino connected", "magenta")
