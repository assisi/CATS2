#!/usr/bin/env python

"""
Compute scores
"""

__author__ = "Leo Cazenille"
__license__ = "WTFPL"
__version__ = "0.1"

########### IMPORTS AND BASE GLOBALS ########### {{{1

import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np
import pydot
import seaborn as sns
sns.set_style("ticks")




########### MAIN ########### {{{1
if __name__ == "__main__":
	from optparse import OptionParser
	usage = "%prog [command] [options]"
	parser = OptionParser(usage=usage)

	parser.add_option("-r", "--referenceDir", dest = "referenceDir", default = "",
			help = "Path of reference input data files")
	parser.add_option("-i", "--inputDir", dest = "inputDir", default = "",
			help = "Path of input data files")
	parser.add_option("-o", "--outputDir", dest = "outputDir", default = "plots/",
			help = "Directory of output plots")
	parser.add_option("-a", "--arenaFilename", dest = "arenaFilename", default = "FishModel/arenas/SetupLargeModelzoneBlack70x70v2.png",
			help = "Filename of arena map file")
	parser.add_option("-z", "--nbZones", dest = "nbZones", default = 6,
			help = "Number of zones")
	parser.add_option("-k", "--skip", dest = "skip", default = 0,
			help = "")
#	parser.add_option("-C", "--zonesLabels", dest = "zonesLabels", default = "Void,Corridor,Center of Room 1,Center of Room 2,Walls of Room 1,Walls of Room 2",
#			help = "Labels of all zones, separated by ','")
#	parser.add_option("-s", "--seed", dest = "seed", default = 42,
#			help = "Random number generator seed")
#	parser.add_option("-S", "--nbSteps", dest = "nbSteps", default = 1800,
#			help = "Number of steps per eval")
#	parser.add_option("-n", "--nbRuns", dest = "nbRuns", default = 10,
#			help = "Number of runs per eval")

	(options, args) = parser.parse_args()

	nbZones = int(options.nbZones)
	#zonesNames = options.zonesLabels.split(",")
	#zonesLabels = ["%s\n%.2f%s" % (zonesNames[i], zonesOccupation[i] * 100., '%') for i in range(nbZones)]
	#zonesLabels = zonesLabels[1:]

	from fitness import *
	f = FitnessCalibration(options.referenceDir, mapFilename = options.arenaFilename, nbZones=nbZones, seed=42, nbRuns=1, nbSteps=1, skip=int(options.skip))
	f2 = FitnessCalibration(options.inputDir, mapFilename = options.arenaFilename, nbZones=nbZones, seed=42, nbRuns=1, nbSteps=1, skip=int(options.skip))
	individualScore, allScores = f.computeScore(f2.referenceFSet)
	print individualScore
	print allScores




# MODELINE	"{{{1
# vim:noexpandtab:softtabstop=4:shiftwidth=4:fileencoding=utf-8
# vim:foldmethod=marker
