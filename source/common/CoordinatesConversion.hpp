#ifndef CATS2_COORDINATES_CONVERSION_HPP
#define CATS2_COORDINATES_CONVERSION_HPP

#include "CommonPointerTypes.hpp"

#include <QtCore/QString>
#include <QtCore/QSharedPointer>

class PositionPixels;
class PositionMeters;
class OrientationRad;

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
    //! Returns the status of the calibration initialization.
    bool isValid();
    //! Converts the position in pixels to the position in meters.
    PositionMeters imageToWorldPosition(PositionPixels imageCoordinates) const;
    //! Converts the orientation from image to world.
    //! FIXME : this method calls imageToWorldPosition() and so it is not optimal to call both methods, need to be merged somehow
    OrientationRad image2WorldOrientationRad(PositionPixels imageCoordinates, OrientationRad imageOrientation) const;

private:
    //! The object that calibrates the camera and basically makes the job of coordinates conversion.
    CameraCalibrationPtr m_cameraCalibration;
};

#endif // CATS2_COORDINATES_CONVERSION_HPP
