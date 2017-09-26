#!/usr/bin/env python3

import numpy as np
from scipy.misc import imread
import os
import os
from math import *

from collections import Counter
from findGroups import findGroups

import plotMC

__author__ = "Leo Cazenille"
__license__ = "WTFPL"
__version__ = "0.1"



def loadArenaMap(filename):
	arenaMat = imread(filename, flatten=True)
	return arenaMat


def computeOrientation(xList, yList):
	assert len(xList) == len(yList)
	orientationList = []

	for x, y in zip(xList, yList):
		assert x.shape == y.shape
		orientation = np.empty_like(x)
		for i in range(1, x.shape[0] - 1):
			for j in range(x.shape[1]):
				orientation[i,j] = atan2(y[i,j] - y[i-1,j], x[i,j] - x[i-1,j])
				if orientation[i,j] > np.pi:
					orientation[i,j] -= 2. * np.pi
				elif orientation[i,j] < -np.pi:
					orientation[i,j] += 2. * np.pi
		orientationList.append(orientation)
	return orientationList


# 0 : outside arena
def presenceInZones(xList, yList, arenaMat):
	res = []
	for x, y in zip(xList, yList):
		resElem = np.empty(x.shape, dtype=int)
		for t in range(x.shape[0]):
			for a in range(x.shape[1]):
				if np.isnan(x[t,a]) or np.isnan(y[t,a]):
					resElem[t,a] = 0
				else:
					posX = int(x[t,a] * float(arenaMat.shape[0]))
					posY = int(y[t,a] * float(arenaMat.shape[1]))
					resElem[t,a] = int(arenaMat[posX, posY]) / 10
					#print t, a, posX, posY, resElem[t,a]
		res.append(resElem)
		#print "D1"
	#print "D2"
	return res



def zonesOccupation(xList, yList, presenceZonesList, nbZones):
	occupations = np.zeros(nbZones)
	normalization = 0.
	assert len(xList) == len(yList) == len(presenceZonesList)
	for x, y, presenceZones in zip(xList, yList, presenceZonesList):
		assert x.shape == y.shape
		for i in range(0, x.shape[0]):
			for j in range(x.shape[1]):
				p = presenceZones[i,j]
				if p > 0:
					occupations[p] += 1.
					normalization += 1.
	return occupations / normalization



def findSGs(xList, yList, dc = 0.30):
	sgList = []
	for x, y in zip(xList, yList):
		data = []
		for i in range(x.shape[0]):
			d = []
			for j in range(x.shape[1]):
				d.append([x[i,j], y[i,j]])
			data.append(d)
		sgList.append(np.array(findGroups(data, dc, True)))
	return sgList



def meanFractionInSGOfSize(xList, yList, sgList):
	nbAgents = xList[0].shape[1]
	cumulFractionInSGOfSize = []

	for x, y, sg in zip(xList, yList, sgList):
		fractionInSGOfSize = []
		for t in range(x.shape[0]):
			vals = np.array(sg[t])
			vals = vals[vals > 0]
			counter = Counter(vals)
			fractionInSGOfSizeTmp = [ 0. for i in range(1, nbAgents+1) ]
			for indiv in range(nbAgents):
				if not (np.isnan(x[t,indiv]) or np.isnan(y[t,indiv]) or sg[t][indiv] < 0):
					c = counter[sg[t][indiv]] - 1
					fractionInSGOfSizeTmp[c] += 1. / float(len(vals)) #nbAgents
			fractionInSGOfSize.append(fractionInSGOfSizeTmp)
		cumul = np.cumsum(fractionInSGOfSize, axis=0) / float(x.shape[0])
		cumulFractionInSGOfSize.append(cumul)
	cumulFractionInSGOfSize = np.array(cumulFractionInSGOfSize)

	fractions = np.mean(cumulFractionInSGOfSize[:,-1], axis=0)
	#print "meanFractionInSGOfSize: ", fractions
	#stds = np.std(cumulFractionInSGOfSize[:,-1], axis=0)

	return fractions



def _interindivDist(d):
	res = 0.
	n = 0.
	for i in range(len(d)):
		for j in range(i, len(d)):
			x1, y1 = d[i]
			x2, y2 = d[j]
			foo = np.sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2))
			if not np.isnan(foo):
				res += foo
				n += 1.
	if n == 0.:
		return 0.
	else:
		return res / n


def interindivDistInSGs(xList, yList, sgList, nbBins = 10, domain = (0.0, 1.0)):
	nbAgents = xList[0].shape[1]
	dists = [ [] for i in range(0, nbAgents - 1) ]

	for x, y, sg in zip(xList, yList, sgList):
		for t in range(x.shape[0]):
			currentSGs = np.array(sg[t])
			currentSGs = currentSGs[currentSGs > 0]
			counterSGs = Counter(currentSGs)
			distsCurrentSGs = {}

			for k,v in counterSGs.items():
				distsCurrentSGs[k] = []
			for indiv in range(x.shape[1]):
				if not (np.isnan(x[t,indiv]) or np.isnan(y[t,indiv]) or sg[t][indiv] < 0):
					distsCurrentSGs[sg[t][indiv]].append([x[t,indiv],y[t,indiv]])
			for k,v in distsCurrentSGs.items():
				d = _interindivDist(v)
				SGsize = counterSGs[k] - 1
				if SGsize >= 1:
					dists[SGsize-1].append(d)
	#for a in range(len(dists)): # XXX debug
	#	print "interindivDistInSGs: ", "min: ", np.min(dists[a]), "max: ", np.max(dists[a])

	np.seterr(divide='ignore', invalid='ignore')
	hists = [ np.histogram(d, nbBins, domain)[0] / float(len(d)) for d in dists ]
	return hists


def interindivDistInZones(xList, yList, presenceZonesList, nbZones, nbBins = 10, domain = (0.0, 1.0)):
	dists = [ [] for a in range(nbZones) ]

	for x, y, presenceZones in zip(xList, yList, presenceZonesList):
		assert x.shape == y.shape
		for t in range(x.shape[0]):
			agentsInZones = [ [] for a in range(nbZones) ]
			for a in range(presenceZones.shape[1]):
				p = presenceZones[t,a]
				if p > 0:
					agentsInZones[p].append([x[t,a],y[t,a]])
			for i in range(nbZones):
				if len(agentsInZones[i]) > 1:
					d = _interindivDist(agentsInZones[i])
					dists[i].append(d)
	#for a in range(1, nbZones): # XXX debug
	#	print "interindivDistInZones: ", "min: ", np.min(dists[a]), "max: ", np.max(dists[a])

	np.seterr(divide='ignore', invalid='ignore')
	hists = [ np.histogram(d, nbBins, domain)[0] / float(len(d)) for d in dists ]
	return hists







def linearSpeedsInZones(xList, yList, presenceZonesList, nbZones, fps, nbBins = 10, domain = (0.0, 0.03)):
	speeds = [ [] for a in range(nbZones) ]
	assert len(xList) == len(yList) == len(presenceZonesList)

	for x, y, presenceZones in zip(xList, yList, presenceZonesList):
		#print presenceZones
		assert x.shape == y.shape
		for i in range(2, x.shape[0] - 2):
			for j in range(x.shape[1]):
				linearSpeed = sqrt((x[i+2][j] - x[i-2][j])**2. + (y[i+2][j]-y[i-2][j])**2.) / (4./fps)
				p = presenceZones[i,j]
				if not np.isnan(linearSpeed) and p > 0 and linearSpeed >= domain[0] and linearSpeed <= domain[1]:
					speeds[p].append(linearSpeed)
	#for a in range(1, nbZones): # XXX debug
	#	print "linearSpeedsInZones: ", "min: ", np.min(speeds[a]), "max: ", np.max(speeds[a])

	np.seterr(divide='ignore', invalid='ignore')
	hists = [ np.histogram(s, nbBins, domain)[0] / float(len(s)) for s in speeds ]
	return hists

def linearSpeedsInSGOfSize(xList, yList, sgList, fps, nbBins = 10, domain = (0.0, 0.03)):
	nbAgents = xList[0].shape[1]
	speeds = [ [] for a in range(nbAgents) ]

	for x, y, sg in zip(xList, yList, sgList):
		for i in range(2, x.shape[0] - 2):
			currentSGs = np.array(sg[i])
			currentSGs = currentSGs[currentSGs > 0]
			counterSGs = Counter(currentSGs)
			for j in range(x.shape[1]):
				linearSpeed = sqrt((x[i+2][j] - x[i-2][j])**2. + (y[i+2][j]-y[i-2][j])**2.) / (4./fps)
				if not np.isnan(linearSpeed) and linearSpeed >= domain[0] and linearSpeed <= domain[1]:
					SGsize = counterSGs[sg[i,j]] - 1
					speeds[SGsize].append(linearSpeed)
	#for a in range(len(speeds)): # XXX debug
	#	print "linearSpeedsInSGOfSize: ", "min: ", np.min(speeds[a]), "max: ", np.max(speeds[a])

	np.seterr(divide='ignore', invalid='ignore')
	hists = [ np.histogram(s, nbBins, domain)[0] / float(len(s)) for s in speeds ]
	return hists



def angularSpeedsInZones(xList, yList, orientationList, presenceZonesList, nbZones, fps, nbBins = 10, domain = (0.0, 1. * np.pi)):
	speeds = [ [] for a in range(nbZones) ]
	assert len(xList) == len(yList) == len(orientationList) == len(presenceZonesList)

	for x, y, theta, presenceZones in zip(xList, yList, orientationList, presenceZonesList):
		assert x.shape == y.shape
		for i in range(1, x.shape[0] - 1):
			for j in range(x.shape[1]):
				angularSpeed = (theta[i+1,j] - theta[i,j]) / (1./fps)
				p = presenceZones[i,j]
				if not np.isnan(angularSpeed) and p > 0 and angularSpeed >= domain[0] and angularSpeed <= domain[1]:
					speeds[p].append(angularSpeed)
	#for a in range(1, nbZones): # XXX debug
	#	print "angularSpeeds: ", "min: ", np.min(speeds[a]), "max: ", np.max(speeds[a])

	np.seterr(divide='ignore', invalid='ignore')
	hists = [ np.histogram(s, nbBins, domain)[0] / float(len(s)) for s in speeds ]
	return hists

def angularSpeedsInSGOfSize(xList, yList, orientationList, sgList, fps, nbBins = 10, domain = (0.0, 1. * np.pi)):
	nbAgents = xList[0].shape[1]
	speeds = [ [] for a in range(nbAgents) ]

	for x, y, theta, sg in zip(xList, yList, orientationList, sgList):
		for i in range(1, x.shape[0] - 1):
			currentSGs = np.array(sg[i])
			currentSGs = currentSGs[currentSGs > 0]
			counterSGs = Counter(currentSGs)
			for j in range(x.shape[1]):
				angularSpeed = (theta[i+1,j] - theta[i,j]) / (1./fps)
				if not np.isnan(angularSpeed) and angularSpeed >= domain[0] and angularSpeed <= domain[1]:
					SGsize = counterSGs[sg[i,j]] - 1
					speeds[SGsize].append(angularSpeed)
	#for a in range(len(speeds)): # XXX debug
	#	print "angularSpeedsInSGOfSize ", "min: ", np.min(speeds[a]), "max: ", np.max(speeds[a])

	np.seterr(divide='ignore', invalid='ignore')
	hists = [ np.histogram(s, nbBins, domain)[0] / float(len(s)) for s in speeds ]
	return hists


# TODO verify
def polarizationInZones(xList, yList, orientationList, presenceZonesList, nbZones, nbBins = 10, domain = (0.0, 2.*np.pi)):
	polarization = [ [] for i in range(nbZones - 1) ]

	for x, y, theta, presenceZones in zip(xList, yList, orientationList, presenceZonesList):
		for t in range(x.shape[0]):
			agentsInZones = [ [] for a in range(nbZones - 1) ]
			for a in range(presenceZones.shape[1]):
				p = presenceZones[t,a]
				if p > 0:
					agentsInZones[p-1].append(theta[t,a])
			for z in range(nbZones - 1):
				if len(agentsInZones[z]) > 1:
					polSG = agentsInZones[z][0]
					for i in range(1, len(agentsInZones[z])):
						polSG = polSG - agentsInZones[z][i]
						if polSG > np.pi:
							polSG -= 2. * np.pi
						elif polSG < -np.pi:
							polSG += 2. * np.pi
					if not np.isnan(polSG) and polSG >= domain[0] and polSG <= domain[1]:
						polarization[z].append(polSG)
	#for a in range(1, nbZones): # XXX debug
	#	print "polarizationInZones: ", "min: ", np.min(polarization[a]), "max: ", np.max(polarization[a])

	np.seterr(divide='ignore', invalid='ignore')
	hists = [ np.histogram(p, nbBins, domain)[0] / float(len(p)) for p in polarization ]
	return hists


# TODO verify
# TODO polarization by using dot product ?
def polarizationInSGOfSize(xList, yList, orientationList, sgList, nbBins = 10, domain = (0.0, 2.*np.pi)):
	nbAgents = xList[0].shape[1]
	polarization = [ [] for i in range(0, nbAgents - 1) ]

	for x, y, theta, sg in zip(xList, yList, orientationList, sgList):
		for t in range(x.shape[0]):
			currentSGs = np.array(sg[t])
			currentSGs = currentSGs[currentSGs > 0]
			counterSGs = Counter(currentSGs)
			orientationsCurrentSGs = {}

			for k,v in counterSGs.items():
				orientationsCurrentSGs[k] = []
			for indiv in range(x.shape[1]):
				if not (np.isnan(x[t,indiv]) or np.isnan(y[t,indiv]) or sg[t][indiv] < 0):
					orientationsCurrentSGs[sg[t][indiv]].append(theta[t,indiv])
			for k,v in orientationsCurrentSGs.items():
				SGsize = counterSGs[k] - 1
				if SGsize >= 1:
					polSG = v[0]
					for i in range(1, len(v)):
						polSG = polSG - v[i]
						if polSG > np.pi:
							polSG -= 2. * np.pi
						elif polSG < -np.pi:
							polSG += 2. * np.pi
					if not np.isnan(polSG) and polSG >= domain[0] and polSG <= domain[1]:
						polarization[SGsize-1].append(polSG)
	#for a in range(len(polarization)): # XXX debug
	#	print "polarizationInSGOfSize: ", "min: ", np.min(polarization[a]), "max: ", np.max(polarization[a])

	np.seterr(divide='ignore', invalid='ignore')
	hists = [ np.histogram(p, nbBins, domain)[0] / float(len(p)) for p in polarization ]
	return hists


def _countTransitions(data, nbClasses):
	res = np.zeros((nbClasses, nbClasses))
	prev = data[0]
	for d in data:
		res[prev, d] += 1
		prev = d
	return res


def zonesTransitions(xList, yList, presenceZonesList, nbZones):
	counts = np.zeros((nbZones, nbZones))
	for presenceZones in presenceZonesList:
		for i in range(presenceZones.shape[1]):
			counts += _countTransitions(presenceZones[:,i], nbZones)
	#print "counts: ", counts
	transitionMat = plotMC.computeTransitionMatrixFromCountMatrix(counts)
	#print "transitionMat: ", np.array(transitionMat)
	return transitionMat


# NOTE: coordonnees des murs en dur
def distanceToWalls(xList, yList, presenceZonesList, nbZones, nbBins = 10, domain = (0.0, 1.0)):
	dists = [ [] for i in range(nbZones) ]
	for x, y, presenceZones in zip(xList, yList, presenceZonesList):
		for i in range(presenceZones.shape[0]):
			for j in range(presenceZones.shape[1]):
				p = presenceZones[i,j]
				if p == 1: # Corridor
					x_ = x[i,j]
					y_ = y[i,j]
					wall1fstX = 0.315
					wall1fstY = 0.590
					wall1sndX = 0.587
					wall1sndY = 0.314
					wall2fstX = 0.428
					wall2fstY = 0.672
					wall2sndX = 0.690
					wall2sndY = 0.404
					dWall1 = np.abs((wall1sndY - wall1fstY) * x_ - (wall1sndX - wall1fstX) * y_ + wall1sndX * wall1fstY - wall1sndY * wall1fstX) / np.sqrt((wall1sndY - wall1fstY)**2. + (wall1sndX - wall1fstX)**2.)
					dWall2 = np.abs((wall2sndY - wall2fstY) * x_ - (wall2sndX - wall2fstX) * y_ + wall2sndX * wall2fstY - wall2sndY * wall2fstX) / np.sqrt((wall2sndY - wall2fstY)**2. + (wall2sndX - wall2fstX)**2.)
					d = np.nanmin([dWall1, dWall2])
					dists[p].append(d)
				elif p >= 2: # Center or walls
					x_ = x[i,j]
					y_ = y[i,j]
					tmpD = [np.abs(x_ - 0.015), np.abs(y_ - 0.980), np.abs(x_ - 0.430), np.abs(y_ - 0.560), np.abs(x_ - 0.580), np.abs(y_ - 0.390), np.abs(x_ - 0.980), np.abs(y_ - 0.010)]
					d = np.nanmin(tmpD)
					#d = np.min(np.abs(x_ - 0.015), np.abs(y_ - 0.980))
					#d = np.min(np.abs(x_ - 0.430), np.abs(y_ - 0.560))
					#d = np.min(np.abs(x_ - 0.580), np.abs(y_ - 0.390))
					#d = np.min(np.abs(x_ - 0.980), np.abs(y_ - 0.010))
					dists[p].append(d)
	np.seterr(divide='ignore', invalid='ignore')
	hists = [ np.histogram(d, nbBins, domain)[0] / float(len(d)) for d in dists ]
	return hists


# MODELINE	"{{{1
# vim:noexpandtab:softtabstop=4:shiftwidth=4:fileencoding=utf-8
# vim:foldmethod=marker
