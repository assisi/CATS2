#!/usr/bin/env python
# -*- coding: utf-8 -*-

import threading
import time

from settings_interface import Request, CatsSettingsInterface

if __name__ == '__main__':

    settings_interface = CatsSettingsInterface()

    # get variables
    kp = settings_interface.get_value('robots/navigation/pid/kp')[0]
    ki = settings_interface.get_value('robots/navigation/pid/ki')[0]
    kd = settings_interface.get_value('robots/navigation/pid/kd')[0]
    print('Received ' + str(kp) + ';' + str(ki) + ';' + str(kd))

    histogram = settings_interface.get_value('robots/fishModel/ZonedBM/zone_1/speedHistogram')
    print(histogram)
    histogram.append(10)
    settings_interface.set_value('robots/fishModel/ZonedBM/zone_1/speedHistogram', histogram)

    # set variables
    settings_interface.set_value('robots/navigation/pid/kp', [kp * 2])
    settings_interface.set_value('robots/navigation/pid/ki', [ki * 2])
    settings_interface.set_value('robots/navigation/pid/kd', [kd * 2])

    cmd = 'a'
    while cmd != 'q':
        cmd = raw_input('To stop the program press q<Enter>')

    settings_interface.stop_all()


