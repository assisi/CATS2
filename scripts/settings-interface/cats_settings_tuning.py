#!/usr/bin/env python
# -*- coding: utf-8 -*-

import threading
import time

from settings_interface import Request, CatsSettingsInterface

if __name__ == '__main__':

    settings_interface = CatsSettingsInterface()

    # get variables
    kp = settings_interface.get_variable('robots/navigation/pid/kp')
    ki = settings_interface.get_variable('robots/navigation/pid/ki')
    kd = settings_interface.get_variable('robots/navigation/pid/kd')
    print('Received ' + str(kp) + ';' + str(ki) + ';' + str(kd))

    # set variables
    settings_interface.set_variable('robots/navigation/pid/kp', kp * 2)
    settings_interface.set_variable('robots/navigation/pid/ki', ki * 2)
    settings_interface.set_variable('robots/navigation/pid/kd', kd * 2)

    cmd = 'a'
    while cmd != 'q':
        cmd = raw_input('To stop the program press q<Enter>')

    settings_interface.stop_all()


