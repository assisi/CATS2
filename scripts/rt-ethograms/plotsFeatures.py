#!/usr/bin/env python3

"""
Generate plots listing histograms of all relevant features + fitness score
"""

__author__ = "Leo Cazenille"
__license__ = "WTFPL"
__version__ = "0.1"

########### IMPORTS AND BASE GLOBALS ########### {{{1

import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np
#import pydot
import seaborn as sns
sns.set_style("ticks")


########### PLOTS FUNCTIONS ########### {{{1

def hist(outputname, data, nbBins = 10, domain = [0.,1.]):
	fig = plt.figure(figsize=(3,3))
	ax = fig.add_subplot(111)
	color = 'grey'
	#bins = np.linspace(domain[0], domain[1], nbBins+1)
	#ax.bar(data, bins, weights=np.ones_like(data) / len(data), histtype='bar', alpha=0.5, linewidth=1, color = color)
	ax.bar(list(range(nbBins)), data, linewidth=1, color=color)
	#ax.xlim(domain)
	#ax.ylim([0.,1.])
	#plt.axis('off')
	sns.despine()
	plt.tight_layout()
	plt.subplots_adjust(left=0.01, right=0.99, top=0.99, bottom=0.01)
	fig.savefig(outputname)
	plt.close(fig)




########### MAIN ########### {{{1
if __name__ == "__main__":
	from optparse import OptionParser
	usage = "%prog [command] [options]"
	parser = OptionParser(usage=usage)

	parser.add_option("-i", "--inputDir", dest = "inputDir", default = "",
			help = "Path of input data files")
#	parser.add_option("-I", "--bestIndividualFilename", dest = "bestIndividualFilename", default = "",
#			help = "Path of picked model parameter")
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
	f = FitnessCalibration(options.inputDir, mapFilename = options.arenaFilename, nbZones=nbZones, seed=42, nbRuns=1, nbSteps=1, skip=int(options.skip))

	linearSpeedsInZones = f.referenceFSet.getLinearSpeedInZones()
	print("linearSpeedsInZones: ", linearSpeedsInZones[1:])
	for z in range(1,len(linearSpeedsInZones)):
		data = linearSpeedsInZones[z]
		hist(os.path.join(options.outputDir, "hist-zone%i-linearSpeedInZones.pdf" % z), data, f.linearSpeedsInZonesNbBin, f.linearSpeedsInZonesDomain)

	angularSpeedInZones = f.referenceFSet.getAngularSpeedInZones()
	print("angularSpeedInZones: ", angularSpeedInZones[1:])
	for z in range(1,len(angularSpeedInZones)):
		data = angularSpeedInZones[z]
		hist(os.path.join(options.outputDir, "hist-zone%i-angularSpeedInZones.pdf" % z), data, f.angularSpeedsInZonesNbBin, f.angularSpeedsInZonesDomain)

	interindivDistInZones = f.referenceFSet.getInterindivDistInZones()
	print("interindivDistInZones: ", interindivDistInZones[1:])
	for z in range(1,len(interindivDistInZones)):
		data = interindivDistInZones[z]
		hist(os.path.join(options.outputDir, "hist-zone%i-interindivDistInZones.pdf" % z), data, f.interindivDistInZonesNbBin, f.interindivDistInZonesDomain)

#	polarizationInZones = f.referenceFSet.getPolarizationInZones()
#	print("polarizationInZones: ", polarizationInZones)
#	for z in range(0,len(polarizationInZones)):
#		data = polarizationInZones[z]
#		hist(os.path.join(options.outputDir, "hist-zone%i-polarizationInZones.pdf" % (z+1)), data, f.polarizationInZonesNbBin, f.polarizationInZonesDomain)

	# Distance to walls
	distToWallsByZones = f.referenceFSet.getDistToWallsByZones()
	print("distToWallsByZones: ", distToWallsByZones[1:])
	for z in range(1,len(distToWallsByZones)):
		data = distToWallsByZones[z]
		hist(os.path.join(options.outputDir, "hist-zone%i-distToWallsByZones.pdf" % (z)), data, f.distToWallsByZonesNbBin, f.distToWallsByZonesDomain)


# MODELINE	"{{{1
# vim:noexpandtab:softtabstop=4:shiftwidth=4:fileencoding=utf-8
# vim:foldmethod=marker
