# -*- coding: utf-8 -*-
"""
Created on Sun Jul 08 17:05:20 2018

@author: joell
"""
from g_settings import *


'''
These are the settings on the actual mc
    int chip_select_pin; // cant change
    double current_sense_resistor; // cant change
    int torque;
    ENBL_Values enabled;
    ISGAIN_Values current_gain;
    DTIME_Values dead_time;
    int off_time;
    PWMMODE_Values pwm_mode;
    int blanking_time;
    int mixed_decay_time;
    DECMOD_Values decay_mode;
    OCPTH_Values over_current_protection_voltage_threshold;
    OCPDEG_Values over_current_protection_deglitch_time;
    TDRIVEN_Values gate_drive_sink_time;
    TDRIVEP_Values gate_drive_source_time;
    IDRIVEN_Values gate_drive_peak_sink_current;
    IDRIVEP_Values gate_drive_peak_source_current;
    DRV8704_Status status;
'''

class MCD_Settings(QtGui.QWidget):
    def __init__(self): #THIS IS SUPER IMPORTANT
        super(self.__class__, self).__init__() #THIS IS SUPER IMPORTANT
        uic.loadUi('Motor_Driver_Settings.ui', self) #THIS IS WHERE YOU LOAD THE .UI FILE
        global mc, debug_window

        # CTRL TAB

        # TORQUE TAB
        self.spinBox.valueChanged.connect(self.calculate_i_chop)
        self.calculate_i_chop()
        self.dial.valueChanged.connect(self.auto_send_check)


        # OFF TAB
        self.spinBox_2.valueChanged.connect(self.fixed_time_off)

        # BLANK TAB
        self.spinBox_3.valueChanged.connect(self.current_blank_time)

        # DECAY TAB
        self.spinBox_4.valueChanged.connect(self.decay_trans_time)

        # DRIVE TAB

        # Controls
        self.reset_button.clicked.connect(self.reset_all_values)
        self.send_button.clicked.connect(self.send_all_values)

        # STATUS TAB
        self.pushButton.clicked.connect(self.get_status)

        print "starting up the MC"

    def calculate_i_chop(self):
        torque = self.spinBox.value()
        gain_index = self.comboBox_2.currentIndex()
        gain = 0
        if(gain_index == 0):
            gain = 5
        elif(gain_index == 1):
            gain = 10
        elif(gain_index == 2):
            gain = 20
        elif(gain_index == 3):
            gain = 40
        self.label_6.setText("I = {:.4}".format((2.75*torque)/(256.0*gain*0.0075)))

    def get_status(self):
        from g_settings import debug_window,mc
        mc.send_string("Q")
        #print mc.read_data()

    def fixed_time_off(self):
        t_off = self.spinBox_2.value()
        self.label_9.setText("t_off = {} uS".format(0.525+0.525*t_off))

    def current_blank_time(self):
        t_blank = self.spinBox_3.value()
        self.label_13.setText("Tblank = {} uS".format(0.5+t_blank*0.02))

    def decay_trans_time(self):
        trans_time = self.spinBox_4.getValue()
        self.label_17.setText("T_Decay = {} uS".format(trans_time*0.525))

    def reset_all_values(self):
        self.comboBox.setCurrentIndex(1)
        self.comboBox_2.setCurrentIndex(3)
        self.comboBox_3.setCurrentIndex(0)

        self.spinBox.setValue(255)

        self.spinBox_2.setValue(48)

        self.spinBox_3.setValue(128)

        self.spinBox_4.setValue(16)
        self.comboBox_4.setCurrentIndex(0)

        self.comboBox_5.setCurrentIndex(1)
        self.comboBox_6.setCurrentIndex(1)
        self.comboBox_7.setCurrentIndex(2)
        self.comboBox_19.setCurrentIndex(2)
        self.comboBox_9.setCurrentIndex(3)
        self.comboBox_10.setCurrentIndex(3)
        from g_settings import debug_window
        debug_window.debug_output("Resetting all of the values")

    def auto_send_check(self):
        if(self.checkBox.isChecked()):
            self.send_all_values()

    def send_all_values(self):
        from g_settings import debug_window, mc
        debug_window.debug_output("SENDING ALL OF THE VALUES")
        debug_window.debug_output("CONTROL TAB: {}".format(self.get_ctrl_tab_data()))
        debug_window.debug_output("TORQUE TAB: {}".format(self.get_torque_tab_data()))
        debug_window.debug_output("OFF TAB: {}".format(self.get_off_tab_data()))
        debug_window.debug_output("BLANK TAB: {}".format(self.get_blank_tab_data()))
        debug_window.debug_output("DECAY TAB: {}".format(self.get_decay_tab_data()))
        debug_window.debug_output("DRIVE TAB: {}".format(self.get_drive_tab_data()))
        sts = ""
        ctrl = self.get_ctrl_tab_data()
        sts += "{},{},{}".format(int(str(ctrl[0]), base=2),
                                 int(str(ctrl[1]), base=2),
                                 int(str(ctrl[2]), base=2))
        sts += ",{}".format(self.get_torque_tab_data()) # fine being 8 bit number
        sts += ",{}".format(self.get_off_tab_data()) # fine being 8 bit number
        sts += ",{}".format(self.get_blank_tab_data()) # fine being 8 bit number

        decay = self.get_decay_tab_data()
        sts += ",{},{}".format(int(decay[0]),  # fine being 8 bit number
                               int(str(decay[1]), base=2))

        drive = self.get_drive_tab_data()
        sts += ",{},{},{},{},{},{}".format(
                                int(str(drive[0]), base=2),
                                int(str(drive[1]), base=2),
                                int(str(drive[2]), base=2),
                                int(str(drive[3]), base=2),
                                int(str(drive[4]), base=2),
                                int(str(drive[5]), base=2))

        debug_window.debug_output(sts)

        mc.send_string("D{}".format(sts))

    def strip_num_from_text(self, input_string):
        #print input_string
        #print int(input_string.split(":")[0])
        return int(input_string.split(":")[0])

    def get_ctrl_tab_data(self):
        data = [0,0,0]
        enable = self.comboBox.currentText()
        current_gain = self.comboBox_2.currentText()
        dead_set_time = self.comboBox_3.currentText()

        data[0] = self.strip_num_from_text(enable)
        data[1] = self.strip_num_from_text(current_gain)
        data[2] = self.strip_num_from_text(dead_set_time)
        return data

    def get_torque_tab_data(self):
        return self.spinBox.value()

    def get_off_tab_data(self):
        return self.spinBox_2.value()

    def get_blank_tab_data(self):
        return self.spinBox_3.value()

    def get_decay_tab_data(self):
        data = [0,0]
        data[0] = self.spinBox_4.value()
        decay_mode = self.comboBox_4.currentText()

        data[1] = self.strip_num_from_text(decay_mode)
        return data

    def get_drive_tab_data(self):
        data = [0,0,0,0,0,0]
        ocp_thresh = self.comboBox_5.currentText()
        ocp_time =self.comboBox_6.currentText()
        gd_sink_time = self.comboBox_7.currentText()
        gd_source_time = self.comboBox_19.currentText()
        gd_sink_current = self.comboBox_9.currentText()
        gd_source_current = self.comboBox_10.currentText()

        data[0] = self.strip_num_from_text(ocp_thresh)
        data[1] = self.strip_num_from_text(ocp_time)
        data[2] = self.strip_num_from_text(gd_sink_time)
        data[3] = self.strip_num_from_text(gd_source_time)
        data[4] = self.strip_num_from_text(gd_sink_current)
        data[5] = self.strip_num_from_text(gd_source_current)

        return data




if (__name__ == "__main__"):
    import sys  # We need sys so that we can pass argv to QApplication
    app = QtGui.QApplication(sys.argv)  # A new instance of QApplication
    form = MC_Settings()  # We set the form to be our ExampleApp (design)
    form.show()  # Show the form
    app.exec_()  # and execute the app
