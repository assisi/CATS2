#!/usr/bin/env python

"""
Find groups in population using only spatial coordinates as feature
"""

__author__ = "Leo Cazenille"
__license__ = "WTFPL"
__version__ = "0.1"

########### IMPORTS AND BASE GLOBALS ########### {{{1
import logging as log
import numpy as np
import matplotlib.pyplot as plt
import weakref
#from numba import jit
from spatialClustering import densityClustering
from spatialClustering import spatialLocalityClustering
#from convex import *

# Init logging
_logger = log.getLogger("findGroups")
_logger.setLevel(log.DEBUG)
_consoleLogHandler = log.StreamHandler()
_consoleLogHandler.setLevel(log.DEBUG)
_logger.addHandler(_consoleLogHandler)

plt.ion()

# }}}


########### GROUPS ########### {{{1

def findGroups(data, dc = 0.20, newClusterOnTransition = False):
	histLabels = []
	histNormalizedLabels = []
	histNbClusters = 0

	for t in data:
		# Compute labels
		labels = np.array(spatialLocalityClustering(t, dc))

		# Save and normalize labels
		if not histLabels:
			histLabels.append(labels)
			nbClusters = len(set(labels))
			normalizedLabels = np.full(labels.shape, -2, dtype = np.int32)
			clusters = []
			for c in range(nbClusters):
				appartenanceCindexes = np.where(labels == c)[0]
				cItems = labels[appartenanceCindexes]
				if len(appartenanceCindexes):
					if newClusterOnTransition:
						clusterId = 1 + histNbClusters
					else:
						clusterId = int("".join([str(x + 1) for x in appartenanceCindexes]))
					normalizedLabels[appartenanceCindexes] = clusterId
					histNbClusters += 1
			histNormalizedLabels.append(normalizedLabels)

		else:
			prevLabels = histLabels[-1]
			prevNormalizedLabels = histNormalizedLabels[-1]
			histLabels.append(labels)
			if np.all(prevLabels == labels):
				normalizedLabels = prevNormalizedLabels
			else:
				nbClusters = len(set(np.concatenate((prevLabels, labels))))
				normalizedLabels = np.full(labels.shape, -2, dtype = np.int32)
				clusters = []
				for c in range(nbClusters):
					appartenanceCindexes = np.where(labels == c)[0]
					cItems = labels[appartenanceCindexes]
					appartenancePrevCindexes = np.where(prevLabels == c)[0]
					prevcItems = prevLabels[appartenancePrevCindexes]
					if len(appartenanceCindexes):
						if newClusterOnTransition:
							if np.all(appartenanceCindexes == appartenancePrevCindexes):
								clusterId = histNormalizedLabels[-1][appartenanceCindexes[0]]
							else:
								clusterId = 1 + histNbClusters
								histNbClusters += 1
						else:
							clusterId = int("".join([str(x + 1) for x in appartenanceCindexes]))
						normalizedLabels[appartenanceCindexes] = clusterId
			histNormalizedLabels.append(normalizedLabels)

	return histNormalizedLabels


def findGroupsAndSave(filename, outputFileName, frameInterval = (0, 59999), dc = 0.20, nbAgents = 5, useClustersDict = False, nColumn = 3):
	startingFrame, endingFrame = frameInterval
	data_full = np.loadtxt(filename, skiprows=1)[startingFrame:endingFrame, :]
	#foo = np.hstack(data[:, 0::3])
	#avgDetected = len(foo[~np.isnan(foo)]) / float(data.shape[0])
	#nbFrame = startingFrame
	#print "AvgDetected=" + str(avgDetected)
	#data = data.reshape((data.shape[0], data.shape[1] / 3, 3))[:, :, 0:2] / 2040.

	time = data_full[:, 0]
	data = data_full[:, np.sort(np.hstack([np.arange(nbAgents) * nColumn + 1, np.arange(nbAgents) * nColumn + 2]))]

	data2 = []
	for i in range(data.shape[0]):
		d = []
		for j in range(data.shape[1] / 2):
			d.append([data[i,j*2], data[i,j*2+1]])
		data2.append(d)

	#histNormalizedLabels = findGroups(filename, frameInterval, dc, nbAgents, useClustersDict)
	histNormalizedLabels = findGroups(data2, dc, useClustersDict)
	# Save agents position and labels
	np.savetxt(outputFileName, histNormalizedLabels, fmt = '%d')

# }}}


########### MAIN ########### {{{1
if __name__ == "__main__":
	from optparse import OptionParser
	usage = "%prog [command] [options]"
	parser = OptionParser(usage=usage)

	parser.add_option("-i", "--inputFileName", dest = "inputFileName", default = "",
			help = "Path of input data file")
	parser.add_option("-o", "--outputFileName", dest = "outputFileName", default = "",
			help = "Path of output data file")
	parser.add_option("-s", "--startingFrame", dest = "startingFrame", default = "40000",
			help = "Starting frame")
	parser.add_option("-e", "--endingFrame", dest = "endingFrame", default = "59999",
			help = "Ending Frame")
	parser.add_option("-d", "--dc", dest = "dc", default = "0.20",
			help = "dc")
	parser.add_option("-n", "--nbAgents", dest = "nbAgents", default = "5",
			help = "Number of agents")
	parser.add_option("-D", "--findGroupID", action="store_true", dest = "findGroupID", default = False,
			help = "Find Group ID instead of group Type")

	(options, args) = parser.parse_args()

	findGroupsAndSave(options.inputFileName, options.outputFileName, (int(options.startingFrame), int(options.endingFrame)), float(options.dc), int(options.nbAgents), options.findGroupID)
# }}}


# MODELINE	"{{{1
# vim:noexpandtab:softtabstop=4:shiftwidth=4:fileencoding=utf-8
# vim:foldmethod=marker
