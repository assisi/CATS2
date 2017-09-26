#!/usr/bin/env python

"""
Compute fitness
"""

__author__ = "Leo Cazenille"
__license__ = "WTFPL"
__version__ = "0.1"

########### IMPORTS AND BASE GLOBALS ########### {{{1

#from FishModel import model
import numpy as np
from features import *
import sys
import multiprocessing
import scipy.stats.mstats


########### FITNESS COMPUTATION ########### {{{1

def hellingerDistance(data1, data2):
	assert len(data1) == len(data2)
	return np.sqrt(np.sum(np.array([ (np.sqrt(data1[x]) - np.sqrt(data2[x]))**2. for x in range(len(data1))]))) / np.sqrt(2.)


class FeaturesSet(object):
	def __init__(self, xList, yList, simParams, dc = 0.30):
		self.xList = xList
		self.yList = yList
		self.simParams = simParams
		self.dc = dc
		self.orientationList = computeOrientation(self.xList, self.yList)
		self.presenceZonesList = presenceInZones(self.xList, self.yList, self.simParams.arenaMap)
		#self.sgs = findSGs(self.xList, self.yList, self.dc)

	def getZonesOccupation(self):
		if hasattr(self, 'zonesOccupation'):
			return self.zonesOccupation
		res = zonesOccupation(self.xList, self.yList, self.presenceZonesList, self.simParams.nbZones)
		self.zonesOccupation = res
		return res

	def getLinearSpeedInZones(self):
		if hasattr(self, 'linearSpeedsInZones'):
			return self.linearSpeedsInZones
		res = linearSpeedsInZones(self.xList, self.yList, self.presenceZonesList, self.simParams.nbZones, 1.0 / self.simParams.dt, self.simParams.linearSpeedsInSGOfSizeNbBin, self.simParams.linearSpeedsInSGOfSizeDomain)
		self.linearSpeedsInZones = res
		return res

	def getAngularSpeedInZones(self):
		if hasattr(self, 'angularSpeedsInZones'):
			return self.angularSpeedsInZones
		res = angularSpeedsInZones(self.xList, self.yList, self.orientationList, self.presenceZonesList, self.simParams.nbZones, 1.0 / self.simParams.dt, self.simParams.angularSpeedsInZonesNbBin, self.simParams.angularSpeedsInZonesDomain)
		self.angularSpeedsInZones = res
		return res

	def getInterindivDistInZones(self):
		if hasattr(self, 'interindivDistInZones'):
			return self.interindivDistInZones
		res = interindivDistInZones(self.xList, self.yList, self.presenceZonesList, self.simParams.nbZones, self.simParams.interindivDistInZonesNbBin, self.simParams.interindivDistInZonesDomain)
		self.interindivDistInZones = res
		return res

	def getPolarizationInZones(self):
		if hasattr(self, 'polarizationInZones'):
			return self.polarizationInZones
		res = polarizationInZones(self.xList, self.yList, self.orientationList, self.presenceZonesList, self.simParams.nbZones, self.simParams.polarizationInZonesNbBin, self.simParams.polarizationInZonesDomain)
		self.polarizationInZones = res
		return res

	def getZonesTransitions(self):
		if hasattr(self, 'zonesTransitions'):
			return self.zonesTransitions
		res = zonesTransitions(self.xList, self.yList, self.presenceZonesList, self.simParams.nbZones)
		self.zonesTransitions = res
		return res

	def getDistToWallsByZones(self):
		if hasattr(self, 'distToWallsByZones'):
			return self.distToWallsByZones
		res = distanceToWalls(self.xList, self.yList, self.presenceZonesList, self.simParams.nbZones, self.simParams.distToWallsByZonesNbBin, self.simParams.distToWallsByZonesDomain)
		self.distToWallsByZones = res
		return res


#	def getMeanFractionInSGOfSize(self):
#		if hasattr(self, 'meanFractionInSGOfSize'):
#			return self.meanFractionInSGOfSize
#		res = meanFractionInSGOfSize(self.xList, self.yList, self.sgs)
#		self.meanFractionInSGOfSize = res
#		return res
#
#	def getLinearSpeedInSGOfSize(self):
#		if hasattr(self, 'linearSpeedsInSGOfSize'):
#			return self.linearSpeedsInSGOfSize
#		res = linearSpeedsInSGOfSize(self.xList, self.yList, self.sgs, 1.0 / self.simParams.dt, self.simParams.linearSpeedsInSGOfSizeNbBin, self.simParams.linearSpeedsInSGOfSizeDomain)
#		self.linearSpeedsInSGOfSize = res
#		return res
#
#	def getAngularSpeedInSGOfSize(self):
#		if hasattr(self, 'angularSpeedsInSGOfSize'):
#			return self.angularSpeedsInSGOfSize
#		res = angularSpeedsInSGOfSize(self.xList, self.yList, self.orientationList, self.sgs, 1.0 / self.simParams.dt, self.simParams.angularSpeedsInSGOfSizeNbBin, self.simParams.angularSpeedsInSGOfSizeDomain)
#		self.angularSpeedsInSGOfSize = res
#		return res

	def getInterindivDistInSGs(self):
		if hasattr(self, 'interindivDistInSGs'):
			return self.interindivDistInSGs
		res = interindivDistInSGs(self.xList, self.yList, self.sgs, self.simParams.interindivDistInSGsNbBin, self.simParams.interindivDistInSGsDomain)
		self.interindivDistInSGs = res
		return res

#	def getPolarizationInSGOfSize(self):
#		if hasattr(self, 'polarizationInSGOfSize'):
#			return self.polarizationInSGOfSize
#		res = polarizationInSGOfSize(self.xList, self.yList, self.orientationList, self.sgs, self.simParams.polarizationInSGOfSizeNbBin, self.simParams.polarizationInSGOfSizeDomain)
#		self.polarizationInSGOfSize = res
#		return res



class FitnessCalibration(object):
	def __init__(self, referenceDir, nbRuns = 10, mapFilename="FishModel/arenas/arena2roomsLargeSmall.png", nbSteps=1800, dt=1./3., nbFishes=5, nbZones=10, seed=42, skip=0):
		self.referenceDir = referenceDir
		self.nbRuns = nbRuns
		self.mapFilename = mapFilename
		self.nbSteps = nbSteps
		self.dt = dt
		self.nbFishes = nbFishes
		self.nbZones = nbZones
		self.seed = seed + 1
		self.arenaMap = loadArenaMap(mapFilename)
		self.skip = skip
		self.initReference()

	def getDimensionality(self):
		#return self.nbZones * 4 + self.nbZones * self.nbZones;
		# 5 zones
		#return 5 * 4 + 5 + 3 + 3 + 3 + 3
		return 5 * 3 + 5 + 3 + 3 + 3 + 3
		# 3 zones
		#return 3 * 3 + 3 * 3


	def initReference(self):
		xList = []
		yList = []

		# Load xList and yList from data files
		files = []
		for f in os.listdir(self.referenceDir):
			if f.endswith(".txt"):
				files.append(f)
		for i in range(0, len(files)):
			filename, fileext = os.path.splitext(files[i])
			with open(os.path.join(self.referenceDir, files[i]), "r") as source:
				data_full = np.loadtxt(source, skiprows = 1)
			time = data_full[:, 0]
			data = data_full[:, np.sort(np.hstack([np.arange(self.nbFishes) * 3 + 1, np.arange(self.nbFishes) * 3 + 2]))]
			if self.skip == 0:
				xList.append(data[:, 0 : 2 * self.nbFishes : 2])
				yList.append(data[:, 1 : 2 * self.nbFishes + 1 : 2])
			else:
				xList.append(data[::self.skip, 0 : 2 * self.nbFishes : 2])
				yList.append(data[::self.skip, 1 : 2 * self.nbFishes + 1 : 2])

			#xList.append(data[::15, 0 : 2 * self.nbFishes : 2])
			#yList.append(data[::15, 1 : 2 * self.nbFishes + 1 : 2])
			## XXX DEBUG
			#xList.append(data[:150:15, 0 : 2 * self.nbFishes : 2])
			#yList.append(data[:150:15, 1 : 2 * self.nbFishes + 1 : 2])

		self.referenceFSet = FeaturesSet(xList, yList, self, 0.30)

		# Set domains and nbBins
		self.linearSpeedsInZonesDomain = (0.0, 0.20)
		self.linearSpeedsInZonesNbBin = 10
		self.linearSpeedsInSGOfSizeDomain = (0.0, 0.20)
		self.linearSpeedsInSGOfSizeNbBin = 10
		self.angularSpeedsInZonesDomain = (-1. * np.pi, 1. * np.pi)
		self.angularSpeedsInZonesNbBin = 10
		self.angularSpeedsInSGOfSizeDomain = (-1. * np.pi, 1. * np.pi)
		self.angularSpeedsInSGOfSizeNbBin = 10
		self.interindivDistInZonesDomain = (0.0, 0.25)
		self.interindivDistInZonesNbBin = 10
		self.interindivDistInSGsDomain = (0.0, 0.25)
		self.interindivDistInSGsNbBin = 10

		self.polarizationInZonesDomain = (-1. * np.pi, 1. * np.pi)
		self.polarizationInZonesNbBin = 10
		self.polarizationInSGOfSizeDomain = (-1. * np.pi, 1. * np.pi)
		self.polarizationInSGOfSizeNbBin = 10

		self.distToWallsByZonesDomain = (0.0, 0.20)
		self.distToWallsByZonesNbBin = 10


	def computeScore(self, fSet, verbose=False):
		refZonesOccupation = self.referenceFSet.getZonesOccupation()[1:]
		zonesOccupation = fSet.getZonesOccupation()[1:]
		scoreZonesOccupation = (1. - hellingerDistance(refZonesOccupation, zonesOccupation)) ** 2.
		if verbose:
			print("occupations: ", refZonesOccupation, zonesOccupation)

		refLinearSpeedsInZones = self.referenceFSet.getLinearSpeedInZones()[1:]
		linearSpeedsInZones = fSet.getLinearSpeedInZones()[1:]
		scoresLinearSpeedsInZones = []
		for i in range(len(refLinearSpeedsInZones)):
			scoresLinearSpeedsInZones.append((1. - hellingerDistance(refLinearSpeedsInZones[i], linearSpeedsInZones[i])) ** 2.)
		scoreLinearSpeedsInZones = np.mean(scoresLinearSpeedsInZones)
		if verbose:
			print("linearSpeedsInZones: ", refLinearSpeedsInZones, linearSpeedsInZones)

		refAngularSpeedsInZones = self.referenceFSet.getAngularSpeedInZones()[1:]
		angularSpeedsInZones = fSet.getAngularSpeedInZones()[1:]
		scoresAngularSpeedsInZones = []
		for i in range(len(refAngularSpeedsInZones)):
			scoresAngularSpeedsInZones.append((1. - hellingerDistance(refAngularSpeedsInZones[i], angularSpeedsInZones[i])) ** 2.)
		scoreAngularSpeedsInZones = np.mean(scoresAngularSpeedsInZones)
		if verbose:
			print("angularSpeedsInZones: ", refAngularSpeedsInZones, angularSpeedsInZones)

		refInterindivDistInZones = self.referenceFSet.getInterindivDistInZones()[1:]
		interindivDistInZones = fSet.getInterindivDistInZones()[1:]
		scoresInterindivDistInZones = []
		for i in range(len(refInterindivDistInZones)):
			scoresInterindivDistInZones.append((1. - hellingerDistance(refInterindivDistInZones[i], interindivDistInZones[i])) ** 2.)
		scoreInterindivDistInZones = np.mean(scoresInterindivDistInZones)
		if verbose:
			print("interindivDistInZones: ", refInterindivDistInZones, interindivDistInZones)

		refPolarizationInZones = self.referenceFSet.getPolarizationInZones()
		polarizationInZones = fSet.getPolarizationInZones()
		scoresPolarizationInZones = []
		for i in range(len(refPolarizationInZones)):
			scoresPolarizationInZones.append((1. - hellingerDistance(refPolarizationInZones[i], polarizationInZones[i])) ** 2.)
		scorePolarizationInZones = np.mean(scoresPolarizationInZones)
		if verbose:
			print("polarizationInZones: ", refPolarizationInZones, polarizationInZones)

		refZonesTransitions = np.hstack(self.referenceFSet.getZonesTransitions()[1:,1:])
		zonesTransitions = np.hstack(fSet.getZonesTransitions()[1:,1:])
		scoreZonesTransitions = (1. - hellingerDistance(refZonesTransitions, zonesTransitions)) ** 2.
		if verbose:
			print("transitions: ", refZonesTransitions, zonesTransitions)

		refDistToWallsByZones = self.referenceFSet.getDistToWallsByZones()[1:]
		distToWallsByZones = fSet.getDistToWallsByZones()[1:]
		scoresDistToWallsByZones = []
		for i in range(len(refDistToWallsByZones)):
			scoresDistToWallsByZones.append((1. - hellingerDistance(refDistToWallsByZones[i], distToWallsByZones[i])) ** 2.)
		scoreDistToWallsByZones = np.mean(scoresDistToWallsByZones)
		if verbose:
			print("distToWallsByZones: ", refDistToWallsByZones, distToWallsByZones)


#		refMeanFractionInSGOfSize = self.referenceFSet.getMeanFractionInSGOfSize()
#		meanFractionInSGOfSize = fSet.getMeanFractionInSGOfSize()
#		scoreMeanFractionInSGOfSize = (1. - hellingerDistance(refMeanFractionInSGOfSize, meanFractionInSGOfSize)) ** 2.
#
#		refLinearSpeedsInSGOfSize = self.referenceFSet.getLinearSpeedInSGOfSize()
#		linearSpeedsInSGOfSize = fSet.getLinearSpeedInSGOfSize()
#		scoresLinearSpeedsInSGOfSize = []
#		for i in range(len(refLinearSpeedsInSGOfSize)):
#			scoresLinearSpeedsInSGOfSize.append((1. - hellingerDistance(refLinearSpeedsInSGOfSize[i], linearSpeedsInSGOfSize[i])) ** 2.)
#		scoreLinearSpeedsInSGOfSize = np.mean(scoresLinearSpeedsInSGOfSize)
#
#		refAngularSpeedsInSGOfSize = self.referenceFSet.getAngularSpeedInSGOfSize()
#		angularSpeedsInSGOfSize = fSet.getAngularSpeedInSGOfSize()
#		scoresAngularSpeedsInSGOfSize = []
#		for i in range(len(refAngularSpeedsInSGOfSize)):
#			scoresAngularSpeedsInSGOfSize.append((1. - hellingerDistance(refAngularSpeedsInSGOfSize[i], angularSpeedsInSGOfSize[i])) ** 2.)
#		scoreAngularSpeedsInSGOfSize = np.mean(scoresAngularSpeedsInSGOfSize)

#		refInterindivDistInSGs = self.referenceFSet.getInterindivDistInSGs()
#		interindivDistInSGs = fSet.getInterindivDistInSGs()
#		scoresInterindivDistInSGs = []
#		for i in range(len(refInterindivDistInSGs)):
#			scoresInterindivDistInSGs.append((1. - hellingerDistance(refInterindivDistInSGs[i], interindivDistInSGs[i])) ** 2.)
#		scoreInterindivDistInSGs = np.mean(scoresInterindivDistInSGs)

#		refPolarizationInSGOfSize = self.referenceFSet.getPolarizationInSGOfSize()
#		polarizationInSGOfSize = fSet.getPolarizationInSGOfSize()
#		scoresPolarizationInSGOfSize = []
#		for i in range(len(refPolarizationInSGOfSize)):
#			scoresPolarizationInSGOfSize.append((1. - hellingerDistance(refPolarizationInSGOfSize[i], polarizationInSGOfSize[i])) ** 2.)
#		scorePolarizationInSGOfSize = np.mean(scoresPolarizationInSGOfSize)


##		meanScore = np.nanmean([scoreZonesOccupation, scoreLinearSpeedsInZones, scoreAngularSpeedsInZones, scoreInterindivDistInZones, scorePolarizationInZones, scoreZonesTransitions,  scoreMeanFractionInSGOfSize, scoreLinearSpeedsInSGOfSize, scoreAngularSpeedsInSGOfSize, scoreInterindivDistInSGs, scorePolarizationInSGOfSize])
#		meanScore = np.nanmean([scoreZonesOccupation, scoreLinearSpeedsInZones, scoreAngularSpeedsInZones, scoreInterindivDistInZones, scorePolarizationInZones, scoreZonesTransitions])
#		meanScore = np.nanmean([scoreZonesOccupation, scoreZonesTransitions])
#		meanScore = np.nanmean([scoreZonesOccupation, scoreLinearSpeedsInZones, scoreAngularSpeedsInZones, scoreInterindivDistInZones, scoreZonesTransitions, scoreDistToWallsByZones])
		#meanScore = scipy.stats.mstats.gmean([scoreZonesOccupation, scoreLinearSpeedsInZones, scoreAngularSpeedsInZones, scoreInterindivDistInZones, scoreZonesTransitions, scoreDistToWallsByZones])
		allScores = [scoreZonesOccupation, scoreInterindivDistInZones, scoreZonesTransitions]
		meanScore = scipy.stats.mstats.gmean(allScores)

		if verbose:
			print("SCORE=", meanScore)
			print(" zonesOccupation=", scoreZonesOccupation, " linearSpeedsInZones=", scoreLinearSpeedsInZones, scoresLinearSpeedsInZones, " angularSpeedsInZones=", scoreAngularSpeedsInZones, scoresAngularSpeedsInZones, " interindivDistInZones=", scoreInterindivDistInZones, scoresInterindivDistInZones, " polarizationInZones=", scorePolarizationInZones, scoresPolarizationInZones, " zonesTransitions=", scoreZonesTransitions, " distToWallsByZones=", scoreDistToWallsByZones, scoresDistToWallsByZones)
#			#print(" interindivDistInSGs=", scoresInterindivDistInSGs, " linearSpeedsInSGOfSize=", scoresLinearSpeedsInSGOfSize, " angularSpeedsInSGOfSize=", scoresAngularSpeedsInSGOfSize, " meanFractionInSGOfSize=", scoreMeanFractionInSGOfSize, " polarizationInSGOfSize=", scoresPolarizationInSGOfSize)

		return meanScore, allScores


	def indivToParams(self, individual):
		nbAccessibleZones = self.nbZones - 1
		res = {}
		res['kappa0'] = np.abs(individual[0 * nbAccessibleZones : nbAccessibleZones * 1])  * 30.
		res['kappaf'] = np.abs(individual[1 * nbAccessibleZones : nbAccessibleZones * 2])  * 30.
		#res['kappaw'] = np.abs(individual[2 * nbAccessibleZones : nbAccessibleZones * 3])  * 30.
		res['alpha'] = np.abs(individual[2 * nbAccessibleZones : nbAccessibleZones * 3]) * 50. # XXX
		#res['beta'] = np.abs(individual[4 * nbAccessibleZones : nbAccessibleZones * 5]) * 100. # XXX
		#res['gamma'] = np.abs(individual[4 * nbAccessibleZones : nbAccessibleZones * 5]) * 100. # XXX

		## 3 zones
		#gammaz = np.zeros((self.nbZones, self.nbZones))
		#gammaz[1:,1:] = np.abs(individual[3 * nbAccessibleZones : nbAccessibleZones * 4])
		#gammaz[2:,1:] = np.abs(individual[4 * nbAccessibleZones : nbAccessibleZones * 5])
		#gammaz[3:,1:] = np.abs(individual[5 * nbAccessibleZones : nbAccessibleZones * 6])
		#res['gammaz'] = gammaz[1:,1:]

		# 5 zones
		gammaz = np.zeros((self.nbZones, self.nbZones))
		gammaz[1:,1:] = individual[3 * nbAccessibleZones : nbAccessibleZones * 4]
		gammaz[2:,[1,2,4]] = individual[4 * nbAccessibleZones : 4 * nbAccessibleZones + 3]
		gammaz[3:,[1,3,5]] = individual[4 * nbAccessibleZones + 3 : 4 * nbAccessibleZones + 6]
		gammaz[4:,[1,2,4]] = individual[4 * nbAccessibleZones + 6 : 4 * nbAccessibleZones + 9]
		gammaz[5:,[1,3,5]] = individual[4 * nbAccessibleZones + 9 : 4 * nbAccessibleZones + 12]
		res['gammaz'] = gammaz[1:,1:]

		return res


#	# Run model
#	def genTrajectories(self, individual):
#		#xList = []
#		#yList = []
#		nbAccessibleZones = self.nbZones - 1
#
#		kappa0 = np.zeros(self.nbZones) # default: 6.3
#		kappaf = np.zeros(self.nbZones) # default: 20
#		kappaw = np.zeros(self.nbZones)
#		alpha = np.zeros(self.nbZones)
#		beta = np.zeros(self.nbZones)
#		gamma = np.zeros(self.nbZones)
#		gammaz = np.zeros((self.nbZones, self.nbZones))
#
#		kappa0[1:] = np.abs(individual[0 * nbAccessibleZones : nbAccessibleZones * 1])  * 30. # XXX
#		kappaf[1:] = np.abs(individual[1 * nbAccessibleZones : nbAccessibleZones * 2])  * 30. # XXX
#		#kappaw[1:] = np.abs(individual[2 * nbAccessibleZones : nbAccessibleZones * 3])  * 30. # XXX
#		#print("DEBUG1", kappa0, kappaf, kappaw)
#		alpha[1:] = np.abs(individual[2 * nbAccessibleZones : nbAccessibleZones * 3]) * 50. # XXX
#		#beta[1:]  = np.abs(individual[4 * nbAccessibleZones : nbAccessibleZones * 5]) * 100. # XXX
#		#gamma[1:] = np.abs(individual[4 * nbAccessibleZones : nbAccessibleZones * 5]) * 100. # XXX
#
#		# 5 zones
#		gammaz[1:,1:] = individual[3 * nbAccessibleZones : nbAccessibleZones * 4]
#		gammaz[2:,[1,2,4]] = individual[4 * nbAccessibleZones : 4 * nbAccessibleZones + 3]
#		gammaz[3:,[1,3,5]] = individual[4 * nbAccessibleZones + 3 : 4 * nbAccessibleZones + 6]
#		gammaz[4:,[1,2,4]] = individual[4 * nbAccessibleZones + 6 : 4 * nbAccessibleZones + 9]
#		gammaz[5:,[1,3,5]] = individual[4 * nbAccessibleZones + 9 : 4 * nbAccessibleZones + 12]
#
#		## 3 zones
#		#gammaz[1:,1:] = np.abs(individual[3 * nbAccessibleZones : nbAccessibleZones * 4])
#		#gammaz[2:,1:] = np.abs(individual[4 * nbAccessibleZones : nbAccessibleZones * 5])
#		#gammaz[3:,1:] = np.abs(individual[5 * nbAccessibleZones : nbAccessibleZones * 6])
#
#		speedHistogram = self.referenceFSet.getLinearSpeedInZones()
#		#print("----------------------")
#		#print(speedHistogram)
#		#print("----------------------")
#
#		#wall1fstX = 0.315
#		#wall1fstY = 0.590
#		#wall1sndX = 0.587
#		#wall1sndY = 0.314
#		#wall2fstX = 0.428
#		#wall2fstY = 0.672
#		#wall2sndX = 0.690
#		#wall2sndY = 0.404
#		#d = np.min(np.abs(x_ - 0.015), np.abs(y_ - 0.980))
#		#d = np.min(np.abs(x_ - 0.430), np.abs(y_ - 0.560))
#		#d = np.min(np.abs(x_ - 0.580), np.abs(y_ - 0.390))
#		#d = np.min(np.abs(x_ - 0.980), np.abs(y_ - 0.010))
#		wallsCoord = np.empty((10, 2, 2))
#		wallsCoord[0] = [[0.015, 0.980], [0.430, 0.980]]
#		wallsCoord[1] = [[0.430, 0.980], [0.430, 0.672]]
#		wallsCoord[2] = [[0.430, 0.672], [0.690, 0.390]]
#		wallsCoord[3] = [[0.690, 0.390], [0.980, 0.390]]
#		wallsCoord[4] = [[0.980, 0.390], [0.980, 0.010]]
#		wallsCoord[5] = [[0.980, 0.010], [0.580, 0.010]]
#		wallsCoord[6] = [[0.580, 0.010], [0.580, 0.314]]
#		wallsCoord[7] = [[0.580, 0.314], [0.315, 0.560]]
#		wallsCoord[8] = [[0.315, 0.560], [0.015, 0.560]]
#		wallsCoord[9] = [[0.015, 0.560], [0.015, 0.980]]
#		#wallsCoord = [] # XXX
#
#		wallsDirectionCoord = np.empty((10, 2, 2))
#		wallsDirectionCoord[0] = [[0.015, 0.980], [0.430, 0.980]]
#		wallsDirectionCoord[1] = [[0.430, 0.980], [0.430, 0.672]]
#		wallsDirectionCoord[2] = [[0.430, 0.672], [0.690, 0.390]]
#		wallsDirectionCoord[3] = [[0.690, 0.390], [0.980, 0.390]]
#		wallsDirectionCoord[4] = [[0.980, 0.390], [0.980, 0.010]]
#		wallsDirectionCoord[5] = [[0.980, 0.010], [0.580, 0.010]]
#		wallsDirectionCoord[6] = [[0.580, 0.010], [0.580, 0.314]]
#		wallsDirectionCoord[7] = [[0.580, 0.314], [0.315, 0.560]]
#		wallsDirectionCoord[8] = [[0.315, 0.560], [0.015, 0.560]]
#		wallsDirectionCoord[9] = [[0.015, 0.560], [0.015, 0.980]]
#
#		#followWalls = [False, True, False, True] # XXX
#		followWalls = [False, True, False, False, True, True] # XXX
#
#		res = []
#		for run in range(self.nbRuns):
#			seed_ = np.random.randint(90000) + int(np.sum(multiprocessing.current_process()._identity)) #np.abs(np.random.randint(np.iinfo('uint32').max)) #self.seed + run
#			#data = model.runZonedModel(mapFilename=self.mapFilename, nbSteps=self.nbSteps, dt=self.dt, nbFishes=self.nbFishes, nbZones=self.nbZones, seed=seed_, kappa0=kappa0, kappaf=kappaf, alpha=alpha, gamma=gamma, gammaz=gammaz)
#			#print("DEBUGmodel: ", self.mapFilename, self.nbSteps, self.dt, self.nbFishes, self.nbZones, seed_, kappa0, kappaf, alpha, gamma, gammaz, speedHistogram, self.linearSpeedsInZonesDomain[0], self.linearSpeedsInZonesDomain[1])
#			data = model.runZonedModel(mapFilename=self.mapFilename, nbSteps=self.nbSteps, dt=self.dt, nbFishes=self.nbFishes, nbZones=self.nbZones, seed=seed_, kappa0=kappa0, kappaf=kappaf, alpha=alpha, gamma=gamma, gammaz=gammaz, speedHistogram=speedHistogram, minSpeed=self.linearSpeedsInZonesDomain[0], maxSpeed=self.linearSpeedsInZonesDomain[1], wallsCoord=wallsCoord, wallsDirectionCoord=wallsDirectionCoord, followWalls=followWalls)
#			#print("DONE DEBUGmodel: ", self.mapFilename, self.nbSteps, self.dt, self.nbFishes, self.nbZones, seed_, kappa0, kappaf, alpha, gamma, gammaz, speedHistogram, self.linearSpeedsInZonesDomain[0], self.linearSpeedsInZonesDomain[1])
#			#data = model.runZonedModel(mapFilename=self.mapFilename, nbSteps=self.nbSteps, dt=self.dt, nbFishes=self.nbFishes, nbZones=self.nbZones, seed=seed_, speedHistogram=speedHistogram, minSpeed=minSpeed, maxSpeed=maxSpeed)
#
#			#time = data[:, 0]
#			#data = data[:, np.sort(np.hstack([np.arange(self.nbFishes) * 3 + 1, np.arange(self.nbFishes) * 3 + 2]))]
#			#xList.append(data[:, 0 : 2 * self.nbFishes : 2])
#			#yList.append(data[:, 1 : 2 * self.nbFishes + 1 : 2])
#			res.append(data)
#
#		#return xList, yList
#		return res


#	def fitness(self, individual):
#		#xList, yList = self.genTrajectories(individual, self.nbRuns, self.seed)
#		datas = self.genTrajectories(individual)
#		xList = []
#		yList = []
#		for data in datas:
#			d = data[:, np.sort(np.hstack([np.arange(self.nbFishes) * 3 + 1, np.arange(self.nbFishes) * 3 + 2]))]
#			xList.append(d[:, 0 : 2 * self.nbFishes : 2])
#			yList.append(d[:, 1 : 2 * self.nbFishes + 1 : 2])
#
#		individualFSet = FeaturesSet(xList, yList, self, 0.30)
#		individualScore, allScores = self.computeScore(individualFSet)
#
#		#speedHistogram2 = individualFSet.getLinearSpeedInZones()
#		#print(speedHistogram2)
#
#		return individualScore, allScores



########### MAIN ########### {{{1
if __name__ == "__main__":
	np.seterr(divide='ignore', invalid='ignore')

	from optparse import OptionParser
	usage = "%prog [command] [options]"
	parser = OptionParser(usage=usage)

	parser.add_option("-i", "--inputDir", dest = "inputDir", default = "",
			help = "Path of input data files")
	parser.add_option("-a", "--arenaFilename", dest = "arenaFilename", default = "FishModel/arenas/SetupLargeModelzoneBlack70x70v2.png",
			help = "Filename of arena map file")
	parser.add_option("-z", "--nbZones", dest = "nbZones", default = 5,
			help = "Number of zones")

	parser.add_option("-s", "--seed", dest = "seed", default = 42,
			help = "Random number generator seed")
	parser.add_option("-n", "--nbRuns", dest = "nbRuns", default = 10,
			help = "Number of runs per eval")

	parser.add_option("-k", "--skip", dest = "skip", default = 0,
			help = "")

#	parser.add_option("-o", "--outputFileName", dest = "outputFileName", default = "",
#			help = "Path of output data file")
#	parser.add_option("-s", "--startingFrame", dest = "startingFrame", default = "40000",
#			help = "Starting frame")
#	parser.add_option("-e", "--endingFrame", dest = "endingFrame", default = "59999",
#			help = "Ending Frame")
#	parser.add_option("-d", "--dc", dest = "dc", default = "0.20",
#			help = "dc")
#	parser.add_option("-n", "--nbAgents", dest = "nbAgents", default = "5",
#			help = "Number of agents")
#	parser.add_option("-D", "--findGroupID", action="store_true", dest = "findGroupID", default = False,
#			help = "Find Group ID instead of group Type")

	(options, args) = parser.parse_args()

	np.random.seed(int(options.seed))

	f = FitnessCalibration(options.inputDir, mapFilename = options.arenaFilename, nbZones=int(options.nbZones), seed=int(options.seed), nbRuns=int(options.nbRuns), skip=int(options.skip))
	res = f.fitness(np.random.random(f.getDimensionality()))
	print("RESULT: ", res)

#	a = model.runZonedModel(mapFilename="FishModel/arenas/arena2roomsLargeSmall.png", nbSteps=1000, dt=1./3., nbFishes=5, nbZones=9, seed=42, kappa0=np.random.random(9))
#	print(a)


# MODELINE	"{{{1
# vim:noexpandtab:softtabstop=4:shiftwidth=4:fileencoding=utf-8
# vim:foldmethod=marker
