#include "CoordinatesConversion.hpp"
#include "AgentState.hpp"
#include "CameraCalibration.hpp"

/*!
 * Constructor. Gets the file name containing the camera calibration data.
 */
CoordinatesConversion::CoordinatesConversion(QString calibrationFileName) :
    m_cameraCalibration(new CameraCalibration(calibrationFileName))
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
PositionMeters CoordinatesConversion::imageToWorldPosition(PositionPixels imageCoordinates)
{
    PositionMeters position;
    if (m_cameraCalibration->isInitialized())
        position = m_cameraCalibration->image2World(imageCoordinates);
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
    if (m_cameraCalibration->isInitialized())
        orientation = m_cameraCalibration->image2WorldOrientationRad(imageCoordinates, imageOrientation);
    else
        orientation.setValid(false);

    return orientation;
}


