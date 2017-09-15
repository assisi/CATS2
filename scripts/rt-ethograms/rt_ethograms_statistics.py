#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import threading
import time
import threading
import numpy as np
from subprocess import Popen
import os
import shutil

from rt_ethograms_statistics_interface import CatsStatisticsInterface


#stop = False
#nbFish = 4
#robotsList = ["F"]
#
#baseDataDirectory = os.path.join("data", )


class DataAnalysisLauncher(object):
	""" Launch data analysis scripts using data received from CATS2 """

	def __init__(self, nbFish = 4, robotsList = ["F"], baseDataDirectory = "data", baseLogDirectory = "logs", basePlotDirectory = "plots", timeout = 60.):
		self.nbFish = nbFish
		self.robotsList = robotsList
		self.baseDataDirectory = baseDataDirectory
		self.baseLogDirectory = baseLogDirectory
		self.basePlotDirectory = basePlotDirectory
		self.timeout = timeout

		self._stopThreads = False
		self._removeOldData()

		self._analysisThread = threading.Thread(target = self._launchDataAnalysis)
		self._currentDataAnalysisScriptHandle = None
		self._logFile = None

	def start(self):
		self._analysisThread.start()

	def stop(self):
		self._stopThreads = True
		while self._analysisThread.isAlive():
			time.sleep(0.1)
		self._closeCurrentDataAnalysisScript()



	def _convertRawData(self, data):
		#sizeX = len(data[0])
		#sizeY = len(data)
		#res = np.empty(sizeX, sizeY)

		dataTimes = []
		dataFish = []
		dataRobots = []
		metaData = {}
		minX = metaData['setup-min-x'] = float(data[0]['setup-min-x'])
		minY = metaData['setup-min-y'] = float(data[0]['setup-min-y'])
		maxX = metaData['setup-max-x'] = float(data[0]['setup-max-x'])
		maxY = metaData['setup-max-y'] = float(data[0]['setup-max-y'])

		#print('data', data)

		for d in data:
			dataTimes.append(float(d['time']))

			coordsFish = []
			for i in range(self.nbFish):
				x = (float(d['fish-' + str(i) + '-x']) - minX) / (np.abs(maxX) + np.abs(minX))
				if x < 0.: x = 0. # XXX refactor, add arenaSizeX option
				if x > 1.: x = 1. # XXX refactor, add arenaSizeX option
				coordsFish.append(x)
				y = (float(d['fish-' + str(i) + '-y']) - minY) / (np.abs(maxY) + np.abs(minY))
				if y < 0.: y = 0. # XXX refactor, add arenaSizeY option
				if y > 1.: y = 1. # XXX refactor, add arenaSizeY option
				coordsFish.append(y)
				coordsFish.append(float(d['fish-' + str(i) + '-direction']))
			dataFish.append(coordsFish)

			coordsRobots = []
			for i in robotsList:
				xR = (float(d['robot-' + str(i) + '-x']) - minX) / (np.abs(maxX) + np.abs(minX))
				if xR < 0.: xR = 0. # XXX refactor, add arenaSizeX option
				if xR > 1.: xR = 1. # XXX refactor, add arenaSizeX option
				coordsRobots.append(xR)
				yR = (float(d['robot-' + str(i) + '-y']) - minY) / (np.abs(maxY) + np.abs(minY))
				if yR < 0.: yR = 0. # XXX refactor, add arenaSizeY option
				if yR > 1.: yR = 1. # XXX refactor, add arenaSizeY option
				coordsRobots.append(yR)
				coordsRobots.append(float(d['robot-' + str(i) + '-direction']))
			dataRobots.append(coordsRobots)

		return dataTimes, dataFish, dataRobots, metaData


	def _saveData(self, data, windowId, filename = "data.txt"):
		dirName = os.path.join(self.baseDataDirectory, "%d" % windowId)
		os.makedirs(dirName)
		fullFilename = os.path.join(dirName, filename)
		np.savetxt(fullFilename, data)


	def _closeCurrentDataAnalysisScript(self):
		""" Close current data analysis script, if it is still running """
		if self._currentDataAnalysisScriptHandle:
			if self._currentDataAnalysisScriptHandle.poll() == None:
				self._currentDataAnalysisScriptHandle.terminate()
		if self._logFile:
			self._logFile.flush()
			self._logFile.close()


	def _launchDataAnalysisScript(self, windowId):
		""" Launch the data analysis script, which will make plots in parallel.
		    Matplotlib does not support threads, so plots scripts should be executed in different python processes. """
		self._closeCurrentDataAnalysisScript()

		dirName = os.path.join(self.baseDataDirectory, "%d" % windowId)
		#self._currentDataAnalysisScriptHandle = Popen(['ls', '-ld', os.path.abspath(dirName)], shell = True)
		logFilename = os.path.join(self.baseLogDirectory, "%d.log" % windowId)
		self._logFile = open(logFilename, "w")
		dataDir = os.path.join(self.baseDataDirectory, "%d" % windowId)
		plotDir = os.path.join(self.basePlotDirectory, "%d" % windowId)
		print(dataDir, plotDir)
		self._currentDataAnalysisScriptHandle = Popen('./makePlots.sh %s %s' % (dataDir, plotDir), shell = True, universal_newlines=True, stdout=self._logFile, stderr=self._logFile)


	def _removeOldData(self):
		shutil.rmtree(self.baseDataDirectory, True)
		shutil.rmtree(self.baseLogDirectory, True)
		shutil.rmtree(self.basePlotDirectory, True)

		os.makedirs(self.baseDataDirectory)
		os.makedirs(self.baseLogDirectory)
		os.makedirs(self.basePlotDirectory)


	def _launchDataAnalysis(self):
		print("Launching Data Analysis Thread.")
		lastTimeStep = 0
		dataTimes = []
		dataRobots = []
		dataFish = []
		metaData = {}
		currentWindowId = 0
		while not self._stopThreads:
			time.sleep(self.timeout)
			newLastTimeStep = statistics_interface.get_hist_size()
			tmpDataTimes, tmpDataFish, tmpDataRobots, metaData = self._convertRawData(statistics_interface.get_hist_values(lastTimeStep, newLastTimeStep))
			lastTimeStep = newLastTimeStep
			dataTimes += tmpDataTimes
			dataFish += tmpDataFish
			dataRobots += tmpDataRobots
			print(len(tmpDataTimes), len(tmpDataFish), len(tmpDataRobots))
			#print('tmpDataTimes', tmpDataTimes)
			#print('tmpDataFish', tmpDataFish)
			#print('tmpDataRobots', tmpDataRobots)

			currentWindowData = np.empty((len(tmpDataTimes), 1 + self.nbFish * 3 + len(self.robotsList) * 3))
			#print(currentWindowData.shape)
			currentWindowData[:,0] = np.array(tmpDataTimes)
			currentWindowData[:,1:1+len(tmpDataFish[0])] = np.array(tmpDataFish)
			currentWindowData[:,1+len(tmpDataFish[0]):] = np.array(tmpDataRobots)
			#print(currentWindowData)

			allData = np.empty((len(dataTimes), 1 + self.nbFish * 3 + len(self.robotsList) * 3))
			#print(allData.shape)
			allData[:,0] = np.array(dataTimes)
			allData[:,1:1+len(dataFish[0])] = np.array(dataFish)
			allData[:,1+len(dataFish[0]):] = np.array(dataRobots)

			self._saveData(allData, currentWindowId, "data.txt")
			self._launchDataAnalysisScript(currentWindowId)

			currentWindowId += 1



def cleanWebsite():
	p = Popen('./cleanWebsite.sh', shell = True, universal_newlines=True)
	p.wait()


if __name__ == '__main__':
	from optparse import OptionParser
	parser = OptionParser()
	parser.add_option("-H", "--hostname", dest = "hostname", help = "Hostname to connect to", default="127.0.0.1")
	parser.add_option("-f", "--nbFish", dest = "nbFish", help = "number of fish", default=4)
	parser.add_option("-r", "--robotsList", dest = "robotsList", help = "List of robots id, separated by spaces", default="F")
	parser.add_option("-d", "--dataDir", dest = "dataDir", help = "Directory to save output data", default="data")
	parser.add_option("-l", "--logDir", dest = "logDir", help = "Directory to save logs", default="logs")
	parser.add_option("-p", "--plotDir", dest = "plotDir", help = "Directory to save plots", default="plots")
	parser.add_option("-t", "--timeout", dest = "timeout", help = "Timeout between analysis rounds", default=60.)


	(options, args) = parser.parse_args()
	hostname = str(options.hostname)
	nbFish = int(options.nbFish)
	robotsList = options.robotsList.split()
	baseDataDirectory = options.dataDir
	baseLogDirectory = options.logDir
	basePlotDirectory = options.plotDir
	timeout = float(options.timeout)

	statistics_interface = CatsStatisticsInterface(hostname)

	# request the list of all available statistics
	all_statistics = statistics_interface.get_statistics_list()
	print('Available statistics: ' + all_statistics)

	# subscribe to the statististics
	statistics_interface.subscribe(all_statistics)

	# Clean website
	cleanWebsite()

	launcher = DataAnalysisLauncher(nbFish, robotsList, baseDataDirectory, baseLogDirectory, basePlotDirectory, timeout)
	launcher.start()

	cmd = 'a'
	while cmd != 'q':
		cmd = raw_input('To stop the program press q<Enter>')

	statistics_interface.stop_all()
	launcher.stop()


