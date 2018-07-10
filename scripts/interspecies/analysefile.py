#!/usr/bin/env python3

import os
import sys
import subprocess as sp
import numpy as np
import scipy.interpolate
from scipy.misc import imread 


class Formattxt:
    """ Fomate txt file """

    def __init__(self, line0, lineF, filename, directory, nfish, nrobot, firstcolumn, ncolumn, moveXY, interpol, fps, scale):

        
        self.directory = directory
        self.fichier = filename
        self.nfish = nfish
        self.nrobot = nrobot
        self.firstcolumn = firstcolumn
        self.ncolumn = ncolumn
        self.moveXY = moveXY
        self.interpol = interpol
        self.line0 = line0
        self.lineF = lineF
        self.fps = fps
        self.scale = scale
        if not os.path.exists(self.directory + '/formatedCATS2'):
            os.makedirs(self.directory + '/formatedCATS2')



        self.filename, self.fileext = os.path.splitext(self.fichier)
        
        print(self.filename)
        
        title='TimeStep'
        for i in range (0, self.nrobot):
            title = title + ' robot' + str(i) + 'X'
            title = title + ' robot' + str(i) + 'Y'
            title = title + ' direction' + str(i)
        for i in range (self.nrobot, self.nfish+self.nrobot):
            title = title + ' fish' + str(i) + 'X'
            title = title + ' fish' + str(i) + 'Y'
            title = title + ' direction' + str(i)

        with open(self.directory + "/" + self.fichier, "rb") as source:
            brut_data_raw = np.genfromtxt(source, skip_header=1)
        
        brut_data = np.copy(brut_data_raw[self.line0:self.lineF,:])

        if self.firstcolumn-1:
            position_data = np.copy(brut_data[:,self.firstcolumn-1:self.ncolumn * (self.nrobot+ self.nfish) + self.firstcolumn-1])      #Modif Yohann "2" remplacer par "options.ncolumn"
        else:
            position_data = np.copy(brut_data)
        ## check data	
        # plt.plot(position_data[:,0],position_data[:,1])
        # plt.show()
        
        if self.moveXY:
            XY = [float(x) for x in self.moveXY.split(',')]
            position_data[:,0::self.ncolumn] = (position_data[:,0::self.ncolumn]-XY[0])/(np.abs(XY[0])+np.abs(XY[2]))
            position_data[:,1::self.ncolumn] = (position_data[:,1::self.ncolumn]-XY[1])/(np.abs(XY[1])+np.abs(XY[3]))


        if self.interpol:
            interval = brut_data[-1,0] - brut_data[0,0]
            print("interval : ",interval)

            newdata = np.zeros((int(interval * self.fps) , int((self.nfish + self.nrobot) * self.ncolumn)))
            for i in range(0, (self.nfish + self.nrobot)):
                for j in range(0, self.ncolumn):
                    fj = scipy.interpolate.interp1d(brut_data[:,0], position_data[:,i*self.ncolumn+j], 'slinear', bounds_error = False)#,fill_value="extrapolate")
                    newdata[:,i*self.ncolumn+j] = fj(np.linspace(0, interval, interval * self.fps))

            position_data = np.copy(newdata)




        ncolexit = self.ncolumn

        
        data = np.ndarray(shape = ( int(interval * self.fps ), int(ncolexit * (self.nfish + self.nrobot) + 1)) )
        data[:,0] = np.linspace(0, interval, interval * self.fps )

        if self.interpol:
            data[:,1:data.shape[1]] = position_data/ self.scale
            np.savetxt(self.directory + '/formatedCATS2/' + self.filename + '_formated.txt', data, fmt='%1.3f', header = title)
        
        else:
            print("PROBLEM")
        # elif brut_data.shape[0]<data.shape[0]: #### Video ou tracking defectueux
        #     print("Bad data")
            
        # elif (brut_data.shape[0]-options.begin)<data.shape[0]: ##### Cas d'une video coupe juste avant la fin
        #     print("Too short !")
        #     data[:,1:data.shape[1]] = (position_data[(brut_data.shape[0]-data.shape[0]):brut_data.shape[0],:])/ options.scale
        #     np.savetxt(options.directory + '/formated/' + filename + 'formated.txt', data, fmt='%1.3f', header = title)
            
        # else:
        #     print("Ok !")
        #     data[:,1:data.shape[1]] = (position_data[options.begin-1:options.begin-1+data.shape[0],:]) / options.scale
        #     np.savetxt(options.directory + '/formated/' + filename + 'formated.txt', data, fmt='%1.3f', header = title)


def mazeexploitmap(data, nfish, mazepng, **kwargs): 
	PosX = data[:, 0 : 2 * nfish : 2]
	#print(PosX)
	PosY = data[:, 1 : 2 * nfish + 1 : 2]
	
	allcolor = imread(os.path.dirname(os.path.realpath(__file__))+'/allcolor.png', flatten=True)[0]
	
	arenaMatrix_ = imread(os.path.dirname(os.path.realpath(__file__))+'/'+mazepng, flatten=True)
	arenaMatrix = np.empty(shape=arenaMatrix_.shape, dtype=bool)
	arenaMatrix[arenaMatrix_ == 0] = False
	arenaMatrix[arenaMatrix_ != 0] = True
	arenaChamber = np.zeros(shape=arenaMatrix_.shape)
	arenaChamber[arenaMatrix_ != 0] = 4
	# 0 = nan
	arenaChamber[arenaMatrix_ == allcolor[2]] = 1 #bleu
	arenaChamber[arenaMatrix_ == allcolor[1]] = 2 #vert
	arenaChamber[arenaMatrix_ == allcolor[3]] = 3 #rouge
	arenaChamber[arenaMatrix_ == allcolor[0]] = 4 #centre = blanc
	arenaChamber[arenaMatrix_ == allcolor[4]] = 5 #sous couloir R1 = cyan
	arenaChamber[arenaMatrix_ == allcolor[5]] = 6 #magenta
	arenaChamber[arenaMatrix_ == allcolor[6]] = 7 #jaune
	

	PosXpx = (PosX*arenaChamber.shape[0]).astype(int)
	PosXpx[PosXpx < 0] = 0
	PosYpx = (PosY*arenaChamber.shape[0]).astype(int)
	PosYpx[PosYpx < 0] = 0
	
	PosXpx[PosXpx>=2040] = 2039
	PosYpx[PosYpx>=2040] = 2039
	#print(PosXpx)
	#print(PosYpx)
	PosR=arenaChamber[PosXpx,PosYpx]
	if np.sum( PosR == 5 ) == 0:
		dynaspot = np.ndarray(shape = (data.shape[0], 3)) ##HACK
	else:
		dynaspot = np.ndarray(shape = (data.shape[0], 7))
	if kwargs.get('zone'):
		dynaspot = np.ndarray(shape = (data.shape[0], kwargs.get('zone')))
	dynaspotnan = np.ndarray(shape = (data.shape[0], 1))
		
	dynaspot[:,0] = np.sum( PosR == 4 ,1)
	dynaspot[:,1] = np.sum( PosR == 1 ,1)
	dynaspot[:,2] = np.sum( PosR == 2 ,1)
	if kwargs.get('zone'):
		print('Yop')
		dynaspot[:,3] = np.sum( PosR == 3 ,1) ##HACK
	if np.sum( PosR == 5 ) != 0:
		dynaspot[:,4] = np.sum( PosR == 5 ,1)
		dynaspot[:,5] = np.sum( PosR == 6 ,1)
		dynaspot[:,6] = np.sum( PosR == 7 ,1)
	

	PosR[PosR==0] = np.nan
	PosR[PosR==4] = 0

	return( PosR)

def analyse_room(fichier, directory, nfish, nrobot, ncolumn, fps, picmaze):

    filename, fileext = os.path.splitext(fichier)

    with open(directory + "/formatedCATS2/" + filename + '_formated.txt', "r") as source:
        data_full = np.loadtxt(source, skiprows = 1)

    time = data_full[:,0]
    data = data_full[:,np.sort(np.hstack([np.arange(nfish+nrobot)*ncolumn + 1, np.arange(nfish+nrobot)*ncolumn + 2]))]

    PosR = mazeexploitmap(data, nfish+nrobot, picmaze)

    SumRoomAllFTarget = np.sum(PosR[:, nrobot:(nfish + nrobot)] == 1) / float(np.sum(~np.isnan(PosR[:, nrobot:(nfish + nrobot)])))

    SumRoomAllFOther = np.sum(PosR[:, nrobot:(nfish + nrobot)] == 2)  / float(np.sum(~np.isnan(PosR[:, nrobot:(nfish + nrobot)])))
    

    return(SumRoomAllFTarget,SumRoomAllFOther)

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("-f", "--file", type = str, dest = "filename", help = ".txt file to analyse", default = "all")
    parser.add_argument("-d", "--directory", type = str, dest = "directory", help = "Directory of the .txt file")
    parser.add_argument("-n", "--nfish", type = int, dest = "nfish", help = "Number of fish", default = 4)
    parser.add_argument("-r", "--nrobot", type = int, dest="nrobot", help="Number of robot", default = 2)
    parser.add_argument("-s", "--scaling", type = float, dest = "scale", help = "Scaling ratio to get data in m", default = 1)
    parser.add_argument("--fps", type = float, dest = "fps", help = "Number of image per second", default = 15)
    #parser.add_argument("-t", "--time", type = "float", dest = "time", help = "Time of observation (in s)")
    #parser.add_argument("-b", "--begin", type = "int", dest = "begin", help = "Frame to begin with")
    parser.add_argument("-c", "--ncolumn", type = int, dest = "ncolumn", help = "Number of column per fish in the data", default = 3)
    parser.add_argument("-a", "--moveXY", dest = "moveXY", help = "Indicate how move XY in array [x0 y0 xmax ymax]", default = "-0.236,-0.259,0.806,0.807")
    parser.add_argument("-p", "--movetime", type = int, dest = "firstcolumn", help = "Indicate the first column that contains position values (first column is 1 !)", default = 2)
    parser.add_argument("-A", "--interpol", type = int, dest = "interpol", help = "Do we need to interpolate ? yes 1 no 0", default = 1)
    parser.add_argument("--start", type = int, dest = "line0")
    parser.add_argument("--stop", type = int, dest = "lineF")
    parser.add_argument("-m", "--picmaze", dest = "picmaze", help = "picture of the maze", default='2RColorLargeIR2.png')

    args = parser.parse_args()

formattxt = Formattxt( args.line0, args.lineF, args.filename, args.directory, args.nfish, args.nrobot, args.firstcolumn, args.ncolumn, args.moveXY, args.interpol, args.fps, args.scale)
fishTimeR1, fishTimeR2 = analyse_room(args.filename, args.directory, args.nfish, args.nrobot, args.ncolumn, args.fps, args.picmaze)
print(fishTimeR1, fishTimeR2)
