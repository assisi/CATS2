#include "CoordinatesConversion.hpp"

#include "CameraCalibration.hpp"

/*!
 * Constructor. Gets the file name containing the camera calibration data.
 */
CoordinatesConversion::CoordinatesConversion(QString calibrationFileName) :
    _cameraCalibration(new CameraCalibration(calibrationFileName))
{

}

/*!
 * Destructor.
 */
CoordinatesConversion::~CoordinatesConversion()
{

}

/*!
 * Converts the position in pixels to the position in meters.
 */
PositionMeters CoordinatesConversion::imageToWorld(PositionPixels imageCoordinates)
{
    PositionMeters position;
    if (_cameraCalibration.data()->isInitialized())
        position = _cameraCalibration.data()->image2World(imageCoordinates);
    else
        position.setValid(false);

    return position;
}

/*!
 * Converts the orientation from image to world.
 */
OrientationRad CoordinatesConversion::image2WorldOrientationRad(PositionPixels imageCoordinates, OrientationRad imageOrientation)
{
    OrientationRad orientation;
    if (_cameraCalibration.data()->isInitialized())
        orientation = _cameraCalibration.data()->image2WorldOrientationRad(imageCoordinates, imageOrientation);
    else
        orientation.setValid(false);

    return orientation;
}


