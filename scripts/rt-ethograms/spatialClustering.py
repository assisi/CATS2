#!/usr/bin/env python

"""
TODO
"""

__author__ = "Leo Cazenille"
__license__ = "WTFPL"
__version__ = "0.1"


########### IMPORTS AND BASE GLOBALS ########### {{{1
import logging as log
import numpy as np
#from numba import jit

# Init logging
_logger = log.getLogger("spatialClustering")
_logger.setLevel(log.DEBUG)
_consoleLogHandler = log.StreamHandler()
_consoleLogHandler.setLevel(log.DEBUG)
_logger.addHandler(_consoleLogHandler)

# }}}


########### CLUSTERING ########### {{{1

#@jit
def minkowskiDistance(data1, data2, n = 2):
	assert len(data1) == len(data2)
	return np.power(np.sum(np.power(np.abs(np.array(data1) - np.array(data2)), float(n))), 1. / n)

#@jit
def euclideanDistance(data1, data2):
	assert len(data1) == len(data2)
	return np.sqrt(np.sum(np.power(np.array(data1) - np.array(data2), 2.)))


def densityClustering(data, dc, distanceFunction = euclideanDistance, *args, **kwargs):
	N = len(data)
	assert N

	# Generate distance matrix
	distMat = np.array([[distanceFunction(x, y) for x in data] for y in data])

	# Compute rhos
	rhos = np.array([len(distMat[x][distMat[x] < dc]) for x in range(N)])
	# Compute deltas
	deltas = np.empty(N)
	for i in range(N):
		d = distMat[x][rhos > rhos[i]]
		if len(d):
			deltas[i] = np.min(d)
		else:
			deltas[i] = np.max(distMat[x])
	normalizedDeltas = deltas / float(np.max(deltas))

	# Determine cluster centers
	labels = np.empty(N, dtype = np.int32)
	labels.fill(-2)
	minDeltaForCenters = kwargs.get('minDeltaForCenters', 0.30)
	maxRhoForOutliers = kwargs.get('maxRhoForOutliers', 0)
	tmpDeltas = normalizedDeltas >= minDeltaForCenters
	nbCenters = np.sum(tmpDeltas)
	labels[tmpDeltas] = list(range(nbCenters))
	if kwargs.get('uniqueLabelForOutliers', False):
		tmpOutliers = np.logical_and(normalizedDeltas >= minDeltaForCenters, rhos <= maxRhoForOutliers)
		#nbOutliers = np.sum(tmpOutliers)
		labels[tmpOutliers] = -1

	# Label assignation
	indexes = [(x, y) for x in range(N) for y in range(x, N) if x != y]
	sortedIndexes = sorted(indexes, key = lambda x_y: distMat[x_y[0], x_y[1]])
	while True:
		quit = True
		for (x, y) in sortedIndexes:
			if labels[x] >= 0 and labels[y] == -2 and rhos[x] > rhos[y]:
				labels[y] = labels[x]
				quit = False
			elif labels[x] == -2 and labels[y] >= 0 and rhos[x] < rhos[y]:
				labels[x] = labels[y]
				quit = False
		if quit:
			break

	return labels, rhos, normalizedDeltas


def spatialLocalityClustering(data, dc, distanceFunction = euclideanDistance, *args, **kwargs):
	N = len(data)
	assert N

	# Generate distance matrix
	distMat = np.array([[distanceFunction(x, y) for x in data] for y in data])

	indexes = [(x, y) for x in range(N) for y in range(x, N) if x != y]
	sortedIndexes = sorted(indexes, key = lambda x_y: distMat[x_y[0], x_y[1]])

	# Label assignation
	labels = np.empty(N, dtype = np.int32)
	labels.fill(-2)
	nbClusters = 1
	nanIndexes = np.array([np.any(np.isnan(i)) for i in data])
	labels[nanIndexes] = -1
	if np.all(nanIndexes): # All data is nan, skip analysis
		return labels
	firstNonNanIndex = np.where(np.logical_not(nanIndexes))[0][0]
	labels[firstNonNanIndex] = 0
	while True:
		quit = True
		for (x, y) in sortedIndexes:
			if not np.isnan(labels[x]) and not np.isnan(labels[y]):
				if labels[x] >= 0 and labels[y] == -2 and distMat[x, y] < dc:
					labels[y] = labels[x]
					quit = False
				elif labels[x] == -2 and labels[y] >= 0 and distMat[x, y] < dc:
					labels[x] = labels[y]
					quit = False

		uninitLabels = (labels == -2)
		if quit == True and np.sum(uninitLabels):
			index = np.where(uninitLabels)[0][0]
			labels[index] = nbClusters
			nbClusters += 1
			quit = False

		if quit:
			break

	return labels

# }}}



########### MAIN ########### {{{1
if __name__ == "__main__":
	from optparse import OptionParser
	usage = "%prog [command] [options]"
	parser = OptionParser(usage=usage)

	# TODO
	#parser.add_option("-i", "--inputFileName", dest = "inputFileName", default = "",
	#		help = "Path of input video file")

	(options, args) = parser.parse_args()

	np.random.seed(132)
	#data = np.random.random((100, 2)) * 100.
	#data = np.random.random((5, 2))
	data = np.array([[0.1, 0.5], [0.2, 0.5], [0.5, 0.5], [0.8, 0.5], [0.9, 0.5]])
	data[:,1] = 0.5
	#dc = 0.15
	#minDeltaForCenters = 0.10
	#maxRhoForOutliers = 0
	#uniqueLabelForOutliers = True
	#labels, rhos, deltas = densityClustering(data, dc, minDeltaForCenters = minDeltaForCenters, maxRhoForOutliers = maxRhoForOutliers, uniqueLabelForOutliers = uniqueLabelForOutliers)
	#print(labels)
	#print(rhos)
	#print(deltas)

	labels = spatialLocalityClustering(data, 0.15)
	labels = labels.astype(np.int32)
	print(labels)

	import matplotlib.pyplot as plt
	fig, ax = plt.subplots()
	ax.set_xlim(0.0, 1.0)
	ax.set_ylim(0.0, 1.0)
	colormap = plt.cm.Set1
	labelsMod = len(set(labels))
	ax.set_color_cycle([colormap(i) for i in np.linspace(0, 0.9, labelsMod)])
	dataPerLabel = [data[labels == i] for i in range(labelsMod)]
	points = [ax.plot(dataPerLabel[i][:,0], dataPerLabel[i][:,1], marker='o', linestyle='None')[0] for i in range(len(dataPerLabel))]
	for i, d in enumerate(dataPerLabel):
		for j in d:
			ax.annotate(str(i), j)
	plt.show()


# }}}


# MODELINE	"{{{1
# vim:noexpandtab:softtabstop=4:shiftwidth=4:fileencoding=utf-8
# vim:foldmethod=marker
