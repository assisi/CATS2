#!/usr/bin/env python3

"""
Generate transition matrix and plot (finite-state automata) of labelled data
"""

__author__ = "Leo Cazenille"
__license__ = "WTFPL"
__version__ = "0.1"

########### IMPORTS AND BASE GLOBALS ########### {{{1

import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np
import pydot

def saveTransitionMatrix(filename, mat):
	np.savetxt(filename, mat)

def countTransitions(data, nbClasses):
	"""
	Compute nb of transitions from one class to another. Return matrix of transitions count.
	"""
	res = np.zeros((nbClasses, nbClasses))
	# Count each transition
	prev = data[0]
	for d in data:
		if(d != prev):
			res[prev, d] += 1
		prev = d
	return res

def computeTransitionMatrixFromCountMatrix(counts):
	"""
	Compute Transition Matrix from Count Matrix
	"""
	# Get total nb of transition from every origin point
	res = counts
	nbClasses = len(counts)
	nbTransitions = np.zeros(nbClasses)
	for i in range(len(nbTransitions)):
		nbTransitions[i] = np.sum(res[i])
	# Convert to probabilities
	for i in range(nbClasses):
		for j in range(nbClasses):
			if nbTransitions[i] != 0.0:
				res[i, j] /= nbTransitions[i]
	return res

def computeTransitionMatrix(data, nbClasses):
	"""
	Compute Transition Matrix from data
	"""
	counts = countTransitions(data, nbClasses)
	return computeTransitionMatrixFromCountMatrix(counts)

def _colorTupleToHex(color):
    return r"#" + "".join(["%0.2X" % int(x * 255) for x in color])

def plotMCFromTransMat(mat, outputFile, nbClasses, classLabels, title=None):
	"""
	Generate a fine-state-automata plot from a transition matrix
	"""
	#colors = [ (1,0,0), (0.9020, 0.9020, 0), (0, 0.4, 0), (0, 0.7490, 0.7490), (0.1037, 0.2255, 0.9459), (1.0000, 0.694, 0.3922), (0.4941, 0.4941, 0.4941) ]
	#colorsStr = [_colorTupleToHex(c) for c in colors]

	threshold = 0.05
	# Create graph
	graph = pydot.Dot(graph_type='digraph')
	graph.set_fontname('Helvetica')
	# Add title, if any
	if title != None:
		graph.set_labelloc('t')
		graph.set_label(title)
	# Create nodes
	nodes = []
	for i in range(nbClasses):
		n = pydot.Node(classLabels[i])
		#n.set_shape('box')
		n.set_shape('circle')
		n.set_style('filled')
		n.set_fillcolor('grey')
		#n.set_fillcolor(colorsStr[i])
		#print(colorsStr[i])
		n.set_fontname('Helvetica')
		nodes.append(n)
		graph.add_node(nodes[i])
	# Create edges
	for i in range(nbClasses):
		for j in range(nbClasses):
			if(mat[i, j] > threshold):
				e = pydot.Edge(nodes[i], nodes[j])
				e.set_penwidth(0.3 + mat[i, j] * 10.0)
				e.set_weight(mat[i, j] * 3.0)
				e.set_color('#22222277')
				e.set_label("%.2f" % mat[i, j])
				n.set_fontname('Helvetica')
				graph.add_edge(e)
	# Write graph file
	if outputFile == "":
		import tempfile
		with tempfile.NamedTemporaryFile() as tf:
			graph.write_png(tf.name)
			import pylab
			img = pylab.imread(tf.name)
			pylab.imshow(img)
			pylab.show()
	else:
		graph.write_png(outputFile)


#def plotMC(data, *args, **kwds):
#	"""
#	Generate a fine-state-automata plot from data, by computing the transition matrix
#	"""
#	outputFile = ""
#	if 'outputFile' in kwds:
#		outputFile = kwds['outputFile']
#	nbClasses = len(kwds['classLabels'])
#	mat = None
#	if kwds['transitionMatrixFileName'] != "":
#	    mat = np.loadtxt(kwds['transitionMatrixFileName'])
#	else:
#	    mat = computeTransitionMatrix(data, nbClasses)
#	if kwds['transitionMatrixOutputFileName'] != "":
#	    saveTransitionMatrix(kwds['transitionMatrixOutputFileName'], mat)
#	else:
#	    print(mat)
#	plotMCFromTransMat(mat, outputFile, nbClasses, kwds['classLabels'], kwds['title'])
#
#
#def globalPlotMC(baseDir, *args, **kwds):
#	"""
#	Make an MC plot from all data in baseDir
#	"""
#	import os
#	dataFilesName = kwds['inputFile']
#	outputFile = kwds['outputFile']
#	classLabels = kwds['classLabels']
#	nbClasses = kwds['classNumber']
#	title = kwds['title']
#
#	mat = None
#	if kwds['transitionMatrixFileName'] != "":
#	    mat = np.loadtxt(kwds['transitionMatrixFileName'])
#	else:
#	    counts = np.zeros((nbClasses, nbClasses))
#	    for root,subFolders,files in os.walk(baseDir):
#		    if dataFilesName in files:
#			    fullPath = os.path.join(root, dataFilesName)
#			    data = np.loadtxt(fullPath)
#			    counts += countTransitions(data, nbClasses)
#	    mat = computeTransitionMatrixFromCountMatrix(counts)
#
#	if kwds['transitionMatrixOutputFileName'] != "":
#	    saveTransitionMatrix(kwds['transitionMatrixOutputFileName'], mat)
#	else:
#	    print(mat)
#	plotMCFromTransMat(mat, outputFile, nbClasses, classLabels, title)


########### MAIN ########### {{{1
if __name__ == "__main__":
	from optparse import OptionParser
	usage = "%prog [command] [options]"
	parser = OptionParser(usage=usage)

	parser.add_option("-i", "--inputDir", dest = "inputDir", default = "",
			help = "Path of input data files")
	parser.add_option("-I", "--bestIndividualFilename", dest = "bestIndividualFilename", default = "",
			help = "Path of picked model parameter")
	parser.add_option("-o", "--outputFileName", dest = "outputFileName", default = "output.txt",
			help = "Path of output data file")
	parser.add_option("-a", "--arenaFilename", dest = "arenaFilename", default = "FishModel/arenas/SetupLargeModelzoneBlack70x70v2.png",
			help = "Filename of arena map file")
	parser.add_option("-z", "--nbZones", dest = "nbZones", default = 6,
			help = "Number of zones")
	parser.add_option("-C", "--zonesLabels", dest = "zonesLabels", default = "Void,Corridor,Center of Room 1,Center of Room 2,Walls of Room 1,Walls of Room 2",
			help = "Labels of all zones, separated by ','")
	parser.add_option("-s", "--seed", dest = "seed", default = 42,
			help = "Random number generator seed")
	parser.add_option("-S", "--nbSteps", dest = "nbSteps", default = 1800,
			help = "Number of steps per eval")
	parser.add_option("-n", "--nbRuns", dest = "nbRuns", default = 10,
			help = "Number of runs per eval")
	parser.add_option("-k", "--skip", dest = "skip", default = 0,
			help = "")

	(options, args) = parser.parse_args()

	nbZones = int(options.nbZones)
	seed = int(options.seed)
	from fitness import *
	np.random.seed(seed)
	f = FitnessCalibration(options.inputDir, mapFilename = options.arenaFilename, nbZones=nbZones, seed=seed, nbRuns=int(options.nbRuns), nbSteps=int(options.nbSteps), skip=int(options.skip))

	if options.bestIndividualFilename == "":
		mat = f.referenceFSet.getZonesTransitions()
		mat = mat[1:,1:]
		zonesOccupation = f.referenceFSet.getZonesOccupation()
		#zonesOccupation = [0.0, 0.26983664,  0.11185476,  0.12038991,  0.2473941,   0.25052459] # XXX test
	else:
		import pickle
		bestsIndiv = pickle.load(open(options.bestIndividualFilename, 'r'))['best']
		indiv = bestsIndiv[-1]
		datas = f.genTrajectories(indiv)
		xList = []
		yList = []
		for data in datas:
			d = data[:, np.sort(np.hstack([np.arange(f.nbFishes) * 3 + 1, np.arange(f.nbFishes) * 3 + 2]))]
			xList.append(d[:, 0 : 2 * f.nbFishes : 2])
			yList.append(d[:, 1 : 2 * f.nbFishes + 1 : 2])
		individualFSet = FeaturesSet(xList, yList, f, 0.30)
		mat = individualFSet.getZonesTransitions()
		mat = mat[1:,1:]
		zonesOccupation = individualFSet.getZonesOccupation()
		individualScore, allScores = f.computeScore(individualFSet)
		print("Fitness value: ", individualScore)

	zonesNames = options.zonesLabels.split(",")
	#zonesLabels = ["%s\n%.3f%s" % (zonesNames[i], zonesOccupation[i], '') for i in range(nbZones)]
	zonesLabels = ["%s\n%.1f%s" % (zonesNames[i], zonesOccupation[i] * 100., '%') for i in range(nbZones)]
	zonesLabels = zonesLabels[1:]

	#plotMCFromTransMat(mat, options.outputFileName, int(options.nbZones)-1, zonesLabels)

	pos = ["1,1!", "0,2!", "0,0!", "2,2!", "2,0!"]
	nbClasses = nbZones - 1
	threshold = 0.001
	# Create graph
	graph = pydot.Dot(graph_type='digraph')
	graph.set_fontname('Helvetica')
	graph.set_fontsize(11)
	# Create nodes
	nodes = []
	for i in range(nbClasses):
		n = pydot.Node(zonesLabels[i])
		#n.set_shape('box')
		n.set_shape('ellipse')
		n.set_style('filled')
		n.set_fillcolor('white')
		n.set_fontname('Helvetica')
		n.set_fontsize(11)
		n.set_pos(pos[i])
		nodes.append(n)
		graph.add_node(nodes[i])
	# Create edges
	for i in range(nbClasses):
		for j in range(nbClasses):
			if(mat[i, j] > threshold):
				if i == j and (i == 1 or i == 2):
					e = pydot.Edge(nodes[i].get_name() + ":w", nodes[j].get_name() + ":w")
				else:
					e = pydot.Edge(nodes[i], nodes[j])
				#e.set_penwidth(0.3 + mat[i, j] * 10.0)
				e.set_penwidth(4.0)
				#e.set_weight(mat[i, j] * 3.0)
				e.set_color('#22222277')
				e.set_fontsize(11)
				eLabel = "%.1f%s" % (mat[i, j] * 100., '%')
				e.set_label(eLabel)
				n.set_fontname('Helvetica')
				graph.add_edge(e)
	# Write graph file
	graph.write_png(options.outputFileName, prog='fdp')


# MODELINE	"{{{1
# vim:noexpandtab:softtabstop=4:shiftwidth=4:fileencoding=utf-8
# vim:foldmethod=marker
