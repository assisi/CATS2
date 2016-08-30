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
        position = m_cameraCalibration->image2World(imageCoordinates);
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
        imagePosition = m_cameraCalibration->world2Image(worldCoordinates);
    else
        imagePosition.setValid(false);

    return imagePosition;
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


