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
    qDebug() << "Destroying the object";
}

/*!
 * Converts the position in pixels to the position in meters.
 */
PositionMeters CoordinatesConversion::imageToWorldPosition(PositionPixels imageCoordinates) const
{
    PositionMeters position;
    if (m_cameraCalibration->isInitialized()) {
        position = m_cameraCalibration->imageToWorld(imageCoordinates);
        position.setValid(imageCoordinates.isValid());
    } else {
        qDebug() << "Conversion is not possible as the calibration is not initialized.";
        position.setValid(false);
    }
    return position;
}

/*!
 * Converts the position in meters to the position in pixels.
 */
PositionPixels CoordinatesConversion::worldToImagePosition(PositionMeters worldCoordinates) const
{
    PositionPixels imagePosition;
    if (m_cameraCalibration->isInitialized()) {
        imagePosition = m_cameraCalibration->worldToImage(worldCoordinates);
        imagePosition.setValid(worldCoordinates.isValid());
    } else {
        qDebug() << "Conversion is not possible as the calibration is not initialized.";
        imagePosition.setValid(false);
    }
    return imagePosition;
}

/*!
 * Converts the orientation from image to world.
 */
OrientationRad CoordinatesConversion::imageToWorldOrientation(PositionPixels imageCoordinates, OrientationRad imageOrientation) const
{
    OrientationRad orientation;
    if (m_cameraCalibration->isInitialized() && imageCoordinates.isValid()) {
        orientation = m_cameraCalibration->imageToWorldOrientation(imageCoordinates, imageOrientation);
        orientation.setValid(imageOrientation.isValid());
    } else {
        qDebug() << "Conversion is not possible as the calibration is not initialized.";
        orientation.setValid(false);
    }
    return orientation;
}

/*!
 * Converts the orientation from world to image.
 */
OrientationRad CoordinatesConversion::worldToImageOrientation(PositionMeters worldCoordinates, OrientationRad worldOrientation) const
{
    OrientationRad orientation;
    if (m_cameraCalibration->isInitialized() && worldCoordinates.isValid()) {
        orientation = m_cameraCalibration->worldToImageOrientation(worldCoordinates, worldOrientation);
        orientation.setValid(worldOrientation.isValid());
    } else {
        qDebug() << "Conversion is not possible as the calibration is not initialized.";
        orientation.setValid(false);
    }
    return orientation;
}

/*!
 * Returns the status of the calibration initialization.
 */
bool CoordinatesConversion::isValid()
{
    return m_cameraCalibration->isInitialized();
}


