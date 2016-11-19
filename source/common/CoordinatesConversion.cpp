#include "CoordinatesConversion.hpp"
#include "AgentState.hpp"
#include "CameraCalibration.hpp"

/*!
 * Constructor. Gets the file name containing the camera calibration data and the required video
 * frames size.
 */
CoordinatesConversion::CoordinatesConversion(QString calibrationFileName, QSize targetFrameSize) :
    m_cameraCalibration(new CameraCalibration(calibrationFileName, targetFrameSize))
{

}

/*!
 * Destructor.
 */
CoordinatesConversion::~CoordinatesConversion()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
}

/*!
 * Converts the position in pixels to the position in meters.
 */
PositionMeters CoordinatesConversion::imageToWorldPosition(PositionPixels imageCoordinates) const
{
    PositionMeters position;
    if (m_cameraCalibration->isInitialized() && imageCoordinates.isValid())
        position = m_cameraCalibration->imageToWorld(imageCoordinates);
    else
        position.setValid(false);

    return position;
}

/*!
 * Converts the position in meters to the position in pixels.
 */
PositionPixels CoordinatesConversion::worldToImagePosition(PositionMeters worldCoordinates) const
{
    PositionPixels imagePosition;
    if (m_cameraCalibration->isInitialized() && worldCoordinates.isValid())
        imagePosition = m_cameraCalibration->worldToImage(worldCoordinates);
    else
        imagePosition.setValid(false);

    return imagePosition;
}

/*!
 * Converts the orientation from image to world.
 */
OrientationRad CoordinatesConversion::imageToWorldOrientation(PositionPixels imageCoordinates, OrientationRad imageOrientation) const
{
    OrientationRad orientation;
    if (m_cameraCalibration->isInitialized() && imageCoordinates.isValid() && imageOrientation.isValid())
        orientation = m_cameraCalibration->imageToWorldOrientation(imageCoordinates, imageOrientation);
    else
        orientation.setValid(false);

    return orientation;
}

/*!
 * Converts the orientation from world to image.
 */
OrientationRad CoordinatesConversion::worldToImageOrientation(PositionMeters worldCoordinates, OrientationRad worldOrientation) const
{
    OrientationRad orientation;
    if (m_cameraCalibration->isInitialized() && worldCoordinates.isValid() && worldOrientation.isValid())
        orientation = m_cameraCalibration->worldToImageOrientation(worldCoordinates, worldOrientation);
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


