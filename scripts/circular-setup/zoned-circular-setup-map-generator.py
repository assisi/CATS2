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
            file.write("%s\t%s\n" % (x.pop(0), y.pop(0)))

def computeZonePolygon(externalRadiusPx, internalRadiusPx, angleInterval, xCenterPx, yCenterPx, outputFile):
    anglesRad = [math.radians(angleDeg) for angleDeg in range(0, 361, angleInterval)]
    xNormalized = [math.cos(angleRad) for angleRad in anglesRad]
    yNormalized = [math.sin(angleRad) for angleRad in anglesRad]
    # first compute the setup map
    externalCircleX = [xCenterPx + externalRadiusPx * x for x in xNormalized]
    externalCircleY = [yCenterPx + externalRadiusPx * y for y in yNormalized]
#    save(externalCircleX[:], externalCircleY[:], "%s-outerCircle.txt" % prefix)
    internalCircleX = [xCenterPx + internalRadiusPx * x for x in xNormalized]
    internalCircleY = [yCenterPx + internalRadiusPx * y for y in yNormalized]
#    save(internalCircleX[:], internalCircleY[:], "%s-innerCircle.txt" % prefix)
    polygonCircleX = externalCircleX + list(reversed(internalCircleX))
    polygonCircleY = externalCircleY + list(reversed(internalCircleY))
    save(polygonCircleX[:], polygonCircleY[:], outputFile)

# main() function
def main():
    angleInterval = 5

    # Setup 1
    xCenterPx = 255
    yCenterPx = 242

    # Setup 2
    #xCenterPx = 265
    #yCenterPx = 245

    zone1externalRadiusPx = 135
    zone1internalRadiusPx = 127
    zone2externalRadiusPx = zone1internalRadiusPx
    zone2internalRadiusPx = 104
    zone3externalRadiusPx = zone2internalRadiusPx
    zone3internalRadiusPx = 96

    computeZonePolygon(zone1externalRadiusPx, zone1internalRadiusPx, angleInterval, xCenterPx, yCenterPx, "zone1.txt")
    computeZonePolygon(zone2externalRadiusPx, zone2internalRadiusPx, angleInterval, xCenterPx, yCenterPx, "zone2.txt")
    computeZonePolygon(zone3externalRadiusPx, zone3internalRadiusPx, angleInterval, xCenterPx, yCenterPx, "zone3.txt")

#    # now compute the rooms, for this we add the first points to the beginning
#    # to loop the points
#    internalCircleX.append(internalCircleX[-1])
#    externalCircleX.append(externalCircleX[-1])
#    internalCircleY.append(internalCircleY[-1])
#    externalCircleY.append(externalCircleY[-1])
#    sectorsNumber = 4
#    roomX = []
#    roomY = []
#    sectorLength = int(math.ceil(360 / (angleInterval * sectorsNumber)))
#    startIndex = 0
#    endIndex = sectorLength
#    for sector in range(1, sectorsNumber + 1):
#        roomX = externalCircleX[startIndex:endIndex+1]
#        roomX += reversed(internalCircleX[startIndex:endIndex+1])
#        print(startIndex,endIndex,sectorLength)
#        roomY = externalCircleY[startIndex:endIndex+1]
#        roomY += reversed(internalCircleY[startIndex:endIndex+1])
##        print(len(roomY))
#        save(roomX, roomY, "room-%s.txt" % (sector))
#        startIndex = startIndex + sectorLength
#        endIndex = min(endIndex + sectorLength, 360/angleInterval)

# Call the main() function to begin the program.
if __name__ == '__main__':
    main()
