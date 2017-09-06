#!/usr/bin/env python
# -*- coding: utf-8 -*-

import threading
import time

from rt_ethograms_statistics_interface import CatsStatisticsInterface

if __name__ == '__main__':
	from optparse import OptionParser
	parser = OptionParser()
	parser.add_option("-H", "--hostname", dest = "hostname", help = "Hostname to connect to", default="127.0.0.1")

	(options, args) = parser.parse_args()
	hostname = str(options.hostname)

	statistics_interface = CatsStatisticsInterface(hostname)

	# request the list of all available statistics
	all_statistics = statistics_interface.get_statistics_list()
	print('Available statistics: ' + all_statistics)

	# subscribe to the statististics
	statistics_interface.subscribe(all_statistics)

	cmd = 'a'
	while cmd != 'q':
		cmd = raw_input('To stop the program press q<Enter>')

	statistics_interface.stop_all()


