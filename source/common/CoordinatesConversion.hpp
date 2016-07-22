#ifndef CATS2_COORDINATES_CONVERSION_HPP
#define CATS2_COORDINATES_CONVERSION_HPP

#include "AgentState.hpp"

#include <QtCore/QString>
#include <QtCore/QSharedPointer>

class CameraCalibration;

/*!
 * \brief The class that converts the position from the frame pixels
 * to the world coordinates in meters.
 * NOTE : this class is not thread-safe, so it should be used in the main thread only.
 */
class CoordinatesConversion
{
public:
    //! Constructor. Gets the file name containing the camera calibration data.
    explicit CoordinatesConversion(QString calibrationFileName);
    //! Destructor.
    virtual ~CoordinatesConversion();

public:
    //! Converts the position in pixels to the position in meters.
    PositionMeters imageToWorld(PositionPixels imageCoordinates);
    //! Converts the orientation from image to world.
    OrientationRad image2WorldOrientationRad(PositionPixels imageCoordinates, OrientationRad imageOrientation);

private:
    //! The object that calibrates the camera and basically makes the job of coordinates conversion.
    QSharedPointer<CameraCalibration> m_cameraCalibration;
};

using CoordinatesConversionPtr = QSharedPointer<CoordinatesConversion>;

#endif // CATS2_COORDINATES_CONVERSION_HPP
