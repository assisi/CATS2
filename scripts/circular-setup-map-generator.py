#!/usr/bin/env python

# import modules
import math
import os

def save(x, y, fileName):
    if not os.path.exists("results"):
        os.makedirs("results")
    with open("results/" + fileName, 'w') as file:
        file.write("x\ty\n")
        # NOTE: add a third zero to be compatible with the trajectory-convertor
        while x and y:
            file.write("%s\t%s\t0\n" % (x.pop(0), y.pop(0)))

# main() function
def main():
    angleInterval = 5
    internalRadiusPx = 88
    externalRadiusPx = 132
    xCenterPx = 266
    yCenterPx = 245
    anglesRad = [math.radians(angleDeg) for angleDeg in range(0, 361, angleInterval)]
    xNormalized = [math.cos(angleRad) for angleRad in anglesRad]
    yNormalized = [math.sin(angleRad) for angleRad in anglesRad]
    # first compute the setup map
    externalCircleX = [xCenterPx + externalRadiusPx * x for x in xNormalized]
    externalCircleY = [yCenterPx + externalRadiusPx * y for y in yNormalized]
    save(externalCircleX[:], externalCircleY[:], 'outerCircle.txt')
    internalCircleX = [xCenterPx + internalRadiusPx * x for x in xNormalized]
    internalCircleY = [yCenterPx + internalRadiusPx * y for y in yNormalized]
    save(internalCircleX[:], internalCircleY[:], 'innerCircle.txt')

    # now compute the rooms, for this we add the first points to the beginning
    # to loop the points
    internalCircleX.append(internalCircleX[-1])
    externalCircleX.append(externalCircleX[-1])
    internalCircleY.append(internalCircleY[-1])
    externalCircleY.append(externalCircleY[-1])
    sectorsNumber = 4
    roomX = []
    roomY = []
    sectorLength = int(math.ceil(360 / (angleInterval * sectorsNumber)))
    startIndex = 0
    endIndex = sectorLength
    for sector in range(1, sectorsNumber + 1):
        roomX = externalCircleX[startIndex:endIndex+1]
        roomX += reversed(internalCircleX[startIndex:endIndex+1])
        print(startIndex,endIndex,sectorLength)
        roomY = externalCircleY[startIndex:endIndex+1]
        roomY += reversed(internalCircleY[startIndex:endIndex+1])
#        print(len(roomY))
        save(roomX, roomY, "room-%s.txt" % (sector))
        startIndex = startIndex + sectorLength
        endIndex = min(endIndex + sectorLength, 360/angleInterval)

# Call the main() function to begin the program.
if __name__ == '__main__':
    main()
