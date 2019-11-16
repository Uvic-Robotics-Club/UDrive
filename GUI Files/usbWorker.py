# -*- coding: utf-8 -*-
"""
Created on Sun Jul 08 18:13:08 2018

@author: joell
"""
from g_settings import * # all the global variables

class WorkerSignals(QObject):
    result = pyqtSignal(str)


class Worker(QtCore.QRunnable):
    '''
    WORKER THREAD
    '''
    def __init__(self, *args, **kwargs):
        super(Worker, self).__init__()
        global run_flag, mc
        # Store constructor arguments (re-used for processing)
        self.args = args
        self.kwargs = kwargs
        self.signals = WorkerSignals()
        print "starting up the worker"

    @pyqtSlot()
    def run(self):
        global run_flag, mc
        '''
        This is the loop that polls the USB
        '''
        try:
            while run_flag:
                read_byte = mc.update()
                if(read_byte is not None):
                    self.signals.result.emit(read_byte)
        except Exception as e:
            print "ERROR FROM THE WORKER: |{}|".format(e)