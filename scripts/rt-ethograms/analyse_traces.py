#!/usr/bin/env python

"""
Traces plots
"""

import os
import sys
from math import *
import scipy.stats
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.colors
import seaborn as sns
sns.set_style("ticks")

__author__ = "Leo Cazenille"
__license__ = "WTFPL"
__version__ = "0.1"


def plotSubTrajInGridMultiIndiv(x, y, size, title, outputname, species, nfish, **kwargs):
	#f = plt.figure(figsize=(3,3))
	colorMap = cm.ScalarMappable(norm=matplotlib.colors.Normalize(vmin=0, vmax=x.shape[1] + 2), cmap=plt.get_cmap('Greys'))
	#colorMap = cm.ScalarMappable(norm=matplotlib.colors.Normalize(vmin=0, vmax=x.shape[1] + 2), cmap=plt.get_cmap('Set1'))
	nbSubX = kwargs.get('nbSubX') or 6
	nbSubY = kwargs.get('nbSubY') or 5
	binSize = int(x.shape[0] / (nbSubX * nbSubY))

	#f = plt.figure(figsize=(3*nbSubX,3*nbSubY))
	f = plt.figure(figsize=(10,10))

	for i in range(nbSubX):
		for j in range(nbSubY):
			index = i*nbSubY+j
			ax = f.add_subplot(nbSubX, nbSubY, index+1)
			ax.set_xlim(0, size)
			ax.set_ylim(0, size)
			ax.set_xticks([])
			ax.set_yticks([])

			for k in range(x.shape[1]):
				if kwargs.get('colors') is not None and k < len(kwargs['colors']):
					continue
				else:
					colorVal = colorMap.to_rgba(k+2)
				X = x[binSize*index:binSize*(index+1), k]
				Y = y[binSize*index:binSize*(index+1), k]
				X2 = [1. - X[0]]
				Y2 = [Y[0]]
				for l in range(1, len(X)):
					if abs(X[l] - X[l-1]) > 0.3 or abs(Y[l] - Y[l-1]) > 0.3:
						X2.append(np.nan)
						Y2.append(np.nan)
					else:
						X2.append(1. - X[l])
						Y2.append(Y[l])
				#ax.plot(x[binSize*index:binSize*(index+1), k], y[binSize*index:binSize*(index+1), k], color = colorVal)
				ax.plot(X2, Y2, color = colorVal)

			if kwargs.get('colors') is not None:
				for k in range(min(x.shape[1], len(kwargs['colors']))):
					colorVal = kwargs['colors'][k]
					X = x[binSize*index:binSize*(index+1), k]
					Y = y[binSize*index:binSize*(index+1), k]
					X2 = [1. - X[0]]
					Y2 = [Y[0]]
					for l in range(1, len(X)):
						if abs(X[l] - X[l-1]) > 0.3 or abs(Y[l] - Y[l-1]) > 0.3:
							X2.append(np.nan)
							Y2.append(np.nan)
						else:
							X2.append(1. - X[l])
							Y2.append(Y[l])
					ax.plot(X2, Y2, color = colorVal)

	#plt.title(title, fontsize=25)
	plt.tight_layout(pad=0.01, w_pad=0.01, h_pad=0.01)
	plt.savefig(outputname)
	plt.close()



def analyse_traces(dataDir, plotsDir, arenaSizeX, arenaSizeY, nAnimals, nRobots, title = "", outputSuffix = ".pdf", nColumn = 2):
	nAgents = nAnimals + nRobots

	files = []
	for f in os.listdir(dataDir):
		if f.endswith(".txt"):
			files.append(f)

	filename, fileext = os.path.splitext(files[0])

	with open(os.path.join(dataDir, files[0]), "r") as source:
		data_full = np.loadtxt(source, skiprows = 1)[:250]
	#data_full = data_full[range(4500,5400) + range(8100,9000) + range(24300,25200) + range(26100,27000)]
	time = data_full[:,0]
	data = data_full[:,np.sort(np.hstack([np.arange(nAgents)* nColumn + 1, np.arange(nAgents) * nColumn + 2]))]

	#plottraj(data[:,0], data[:,1], arenaSizeX, title, os.path.join(plotsDir, outputSuffix), "", nAgents)
	if nRobots > 0:
		colors = ['k']
	else:
		colors = []
	#plotSubTrajInGrid(data[:,0], data[:,1], arenaSizeX, title, os.path.join(plotsDir, "TracesFstIndiv-" + outputSuffix), "", nAgents, colors = colors)
	plotSubTrajInGridMultiIndiv(data[:,::2], data[:,1::2], arenaSizeX, title, os.path.join(plotsDir, "Traces-" + outputSuffix), "", nAgents, colors = colors, nbSubX = 1, nbSubY = 1)


if __name__ == "__main__":
	from optparse import OptionParser
	parser = OptionParser()
	parser.add_option("-d", "--inDirectory", dest = "inDirectory", help = "Directory of the data")
	parser.add_option("-o", "--outDirectory", dest = "outDirectory", help = "Directory of the plots")
	parser.add_option("-x", "--sizeX", type = "float", dest = "sizeX", help = "Size of the square arena", default=1)
	parser.add_option("-y", "--sizeY", type = "float", dest = "sizeY", help = "Size of the square arena", default=1)
	parser.add_option("-a", "--nAnimals", type = "int", dest = "nAnimals", help = "Number of Animals")
	parser.add_option("-r", "--nRobots", type = "int", dest = "nRobots", help = "Number of robots")
	parser.add_option("-S", "--outputFilenameSuffix", dest = "outputFilenameSuffix", help = "Output Filename Suffix")
	parser.add_option("-C", "--nColumn", type="int", dest = "nColumn", help = "nColumn", default=3)

	(options, args) = parser.parse_args()

	analyse_traces(options.inDirectory, options.outDirectory, options.sizeX, options.sizeY, options.nAnimals, options.nRobots, "", options.outputFilenameSuffix, options.nColumn)

