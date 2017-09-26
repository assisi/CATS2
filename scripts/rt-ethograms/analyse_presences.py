#!/usr/bin/env python

"""
Make density of presence plots
"""

import os
import sys
#from dataAnalysis import *
#from dataPlot import *
from math import *
import scipy.stats
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable
import seaborn as sns
sns.set_style("ticks")

__author__ = "Leo Cazenille"
__license__ = "WTFPL"
__version__ = "0.1"


# pdf computes the probability of presence of the fish
# ARG_IN :
# position_xy = array containing the position of the fish (fish0X fish0Y fish1X fish1Y...)
# reso = integrer defining the resolution of the square used in the grid (i.e. the function pools positions in squares of size reso cm x reso cm)
# grid = array with size corresponding to the size of the setup divided by the reso
# nfish = number of fish
# ARG_OUT :
# grid/np.sum(grid) = array that contained the probability for a fish to be present in each case
def probability_of_presence(position_xy, reso, my_grid, nfish, **kwargs):
	for i in range(0, position_xy.shape[0]):
		for indiv in range(nfish):
			if np.isnan(position_xy[i, int(2*indiv)]) == False and np.isnan(position_xy[i, int(2*indiv+1)]) == False:
				X = int(position_xy[i, int(2 * indiv + 1)] * 100./reso)
				Y = int(position_xy[i,int(2 * indiv)] * 100./reso)
				if X >= my_grid.shape[0]:
					X = my_grid.shape[0] - 1
				if Y >= my_grid.shape[1]:
					Y = my_grid.shape[1] - 1
				my_grid[X, Y] += 1
	return(my_grid/np.sum(my_grid))



def plotgrid(grid, resolution, outputname, drawCbar = True, cbarFile = None, **kwargs):
	vmin = kwargs.get('vmin')
	vmax = kwargs.get('vmax')

	cmapName = kwargs.get('cmap', '')
	cmap = plt.get_cmap(cmapName)

	fig, ax = plt.subplots(figsize=(8,8))
	#cax = ax.imshow(1000*grid, interpolation='nearest', cmap=getattr(cm, 'Greys'), vmin = vmin, vmax = vmax)
	cax = ax.imshow(1000*grid, interpolation='nearest', cmap=cmap, vmin = vmin, vmax = vmax)

	ax.set_xticks([])
	ax.set_yticks([])
	plt.title(kwargs.get('title') or '', fontsize=24)
	plt.text(26, -2.4, r'x $10^{-3}$', fontsize=20, ha='left', va='center')

	if drawCbar and cbarFile == None:
		divider = make_axes_locatable(ax)
		cax2 = divider.append_axes("right", size="5%", pad=0.05)
		cbar = fig.colorbar(cax, cax=cax2, format="%.2f")
		cbar.ax.tick_params(labelsize=30)
		if vmin or vmax: cbar.set_clim(vmin, vmax)
		for label in cbar.ax.get_yticklabels()[1::2]:
			label.set_visible(False)

	plt.tight_layout()
	fig.savefig(outputname)

	if drawCbar and cbarFile != None:
		plt.gca().set_visible(False)
		fig.set_size_inches(2.0,8,forward=True)
		cax2 = plt.axes([0.05, 0.10, 0.2, 0.80])
		cbar = fig.colorbar(cax, cax=cax2, format="%.2f")
		cbar.ax.tick_params(labelsize=40)
		if vmin or vmax: cbar.set_clim(vmin, vmax)
		for label in cbar.ax.get_yticklabels()[1::2]:
			label.set_visible(False)
		fig.savefig(cbarFile)

	plt.close(fig)




def analyse_presence(dataDir, plotsDir, arenaSizeX, arenaSizeY, nAnimals, nRobots, title, outputFilenameSuffix, nColumn = 3, resolution = 4, cmap = 'Blues', drawCbar = True, **kwargs):
	nAgents = nAnimals + nRobots
	grid = np.zeros(shape = (int(arenaSizeY * 100. / resolution), int(arenaSizeX * 100. / resolution)))
	gridFish = np.zeros(shape = (int(arenaSizeY * 100. / resolution), int(arenaSizeX * 100. / resolution)))
	gridRobots = np.zeros(shape = (int(arenaSizeY * 100. / resolution), int(arenaSizeX * 100. / resolution)))

	files = []
	for f in os.listdir(dataDir):
		if f.endswith(".txt"):
			files.append(f)

	for ii in range(0, len(files)):
		filename, fileext = os.path.splitext(files[ii])

		with open(os.path.join(dataDir, files[ii]), "r") as source:
			data_full = np.loadtxt(source, skiprows = 1)#[::5][:2000]
		time = data_full[:,0]
		data = data_full[:,np.sort(np.hstack([np.arange(nAgents)* nColumn + 1, np.arange(nAgents) * nColumn + 2]))]
		dataFish = data_full[:,np.sort(np.hstack([np.arange(nAnimals)* nColumn + 1, np.arange(nAnimals) * nColumn + 2]))]
		dataRobots = data_full[:,np.sort(np.hstack([np.arange(nAnimals, nAnimals+nRobots)* nColumn + 1, np.arange(nAnimals, nAnimals+nRobots) * nColumn + 2]))]

		grid_manip = np.zeros(shape = (int(arenaSizeY * 100 / resolution), int(arenaSizeX * 100 / resolution)))
		grid_temp = probability_of_presence(data, resolution, grid_manip, nAgents)
		grid += grid_temp

		grid_manipFish = np.zeros(shape = (int(arenaSizeY * 100 / resolution), int(arenaSizeX * 100 / resolution)))
		grid_tempFish = probability_of_presence(dataFish, resolution, grid_manipFish, nAnimals)
		gridFish += grid_tempFish

		grid_manipRobots = np.zeros(shape = (int(arenaSizeY * 100 / resolution), int(arenaSizeX * 100 / resolution)))
		grid_tempRobots = probability_of_presence(dataRobots, resolution, grid_manipRobots, nRobots)
		gridRobots += grid_tempRobots

	# Plots using all agents
	outputnameBatch = os.path.join(plotsDir, "AllAgents-MeanPresence2D" + outputFilenameSuffix)
	cbarFile = os.path.join(plotsDir, "AllAgents-MeanPresence2DCBAR" + outputFilenameSuffix)
	plotgrid(grid/len(files), resolution, outputnameBatch, nmanip = len(files), title=title, vmin=0.0, vmax=4.0, cmap = cmap, drawCbar = drawCbar, cbarFile = cbarFile)

	# Plots using only fish
	outputnameBatch = os.path.join(plotsDir, "FishOnly-MeanPresence2D" + outputFilenameSuffix)
	cbarFile = os.path.join(plotsDir, "FishOnly-MeanPresence2DCBAR" + outputFilenameSuffix)
	plotgrid(gridFish/len(files), resolution, outputnameBatch, nmanip = len(files), title=title, vmin=0.0, vmax=4.0, cmap = cmap, drawCbar = drawCbar, cbarFile = cbarFile)

	# Plots using only fish
	outputnameBatch = os.path.join(plotsDir, "RobotsOnly-MeanPresence2D" + outputFilenameSuffix)
	cbarFile = os.path.join(plotsDir, "RobotsOnly-MeanPresence2DCBAR" + outputFilenameSuffix)
	plotgrid(gridRobots/len(files), resolution, outputnameBatch, nmanip = len(files), title=title, vmin=0.0, vmax=4.0, cmap = cmap, drawCbar = drawCbar, cbarFile = cbarFile)


#	sliceX = np.linspace(0.0, 1.0, grid.shape[0])
#	horizSliceGrid = np.mean(grid[int(grid.shape[0]/3):int(grid.shape[0]/3*2),:], axis=0)
#	fig = plt.figure(figsize=(20,12))
#	ax = fig.add_subplot(111)
#	ax.plot(sliceX, horizSliceGrid)
#	ax.tick_params(axis='both', which='major', labelsize=30)
#	sns.despine()
#	plt.tight_layout()
#	fig.savefig(os.path.join(plotsDir, "MeanPresence2DHorizSlice-" + outputFilenameSuffix))
#	plt.close(fig)
#
#	vertSliceGrid = np.mean(grid[:, int(grid.shape[1]/3):int(grid.shape[1]/3*2)], axis=1)
#	fig = plt.figure(figsize=(20,12))
#	ax = fig.add_subplot(111)
#	ax.plot(sliceX, vertSliceGrid)
#	ax.tick_params(axis='both', which='major', labelsize=30)
#	sns.despine()
#	plt.tight_layout()
#	fig.savefig(os.path.join(plotsDir, "MeanPresence2DVertSlice-" + outputFilenameSuffix))
#	plt.close(fig)



if __name__ == "__main__":
	from optparse import OptionParser
	parser = OptionParser()
	parser.add_option("-d", "--inDirectory", dest = "inDirectory", help = "Directory of the data")
	parser.add_option("-o", "--outDirectory", dest = "outDirectory", help = "Directory of the plots")
	parser.add_option("-x", "--sizeX", type = "float", dest = "sizeX", help = "Size of the square arena", default=1)
	parser.add_option("-y", "--sizeY", type = "float", dest = "sizeY", help = "Size of the square arena", default=1)
	parser.add_option("-a", "--nAnimals", type = "int", dest = "nAnimals", help = "Number of Animals")
	parser.add_option("-r", "--nRobots", type = "int", dest = "nRobots", help = "Number of robots", default=0)
	parser.add_option("-S", "--outputFilenameSuffix", dest = "outputFilenameSuffix", help = "Output Filename Suffix")
	parser.add_option("-C", "--nColumn", type="int", dest = "nColumn", help = "nColumn", default=3)
	parser.add_option("-M", "--cmap", dest = "cmap", help = "cmap", default='Blues')
	parser.add_option("-R", "--resolution", dest = "resolution", help = "resolution", default=4)
	parser.add_option("-B", "--drawCbar", action="store_true", dest="drawCbar", default=False, help="drawCbar")

	(options, args) = parser.parse_args()

	analyse_presence(options.inDirectory, options.outDirectory, float(options.sizeX), float(options.sizeY), int(options.nAnimals), int(options.nRobots), "", options.outputFilenameSuffix, int(options.nColumn), int(options.resolution), cmap = options.cmap, drawCbar = bool(options.drawCbar))


# MODELINE	"{{{1
# vim:noexpandtab:softtabstop=4:shiftwidth=4:fileencoding=utf-8
# vim:foldmethod=marker
