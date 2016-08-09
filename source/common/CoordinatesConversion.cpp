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
PositionMeters CoordinatesConversion::imageToWorldPosition(PositionPixels imageCoordinates) const
{
    PositionMeters position;
    if (m_cameraCalibration->isInitialized() && imageCoordinates.isValid())
        position = m_cameraCalibration->image2World(imageCoordinates);
    else
        position.setValid(false);

    return position;
}

/*!
 * Converts the orientation from image to world.
 */
OrientationRad CoordinatesConversion::image2WorldOrientationRad(PositionPixels imageCoordinates, OrientationRad imageOrientation) const
{
    OrientationRad orientation;
    if (m_cameraCalibration->isInitialized() && imageCoordinates.isValid() && imageOrientation.isValid())
        orientation = m_cameraCalibration->image2WorldOrientationRad(imageCoordinates, imageOrientation);
    else
        orientation.setValid(false);

    return orientation;
}

/*!
 * Returns the status of the calibration initialization.
 */
bool CoordinatesConversion::isValid()
{
    return m_cameraCalibration->isInitialized();
}


