#ifndef CATS2_TRACKER_CALIBRATOR_HPP
#define CATS2_TRACKER_CALIBRATOR_HPP

#include <libtsai.h>

#include <QtCore>
#include <QtGui>
#include <QtXml/QDomDocument>

class PositionPixels;
class PositionMeters;
class OrientationRad;

/*!
 * \brief The type of the camera.
 */
enum class CameraType
{
    C930E, // Logitech Webcam C930e
    UNDEFINED
};

/*!
 * \brief The CameraCalibration class serves to compute the camera's parameters that are
 * later used to convert between image and world coordinates.
 * TODO : Current implementation uses the libtsai library as it is used in the SwisTrack project
 * (https://sourceforge.net/projects/swistrack/). It is known to work poorly when the camera is
 * perpendicular to the tracking plane, and thus need to be replaced  by more appropriate method,
 * potentially from the OpenCV library.
 */
class CameraCalibration
{
public:
    //! Constructor.
    explicit CameraCalibration(QString calibrationFileName);
    //! Destructor.
    virtual ~CameraCalibration() final;

    //! Converts the position in pixels to the position in meters.
    PositionMeters image2World(PositionPixels imageCoordinates);
    //! Converts the orientation at given position from image to world.
    OrientationRad image2WorldOrientationRad(PositionPixels imageCoordinates, OrientationRad imageOrientationRad);

    bool isInitialized() const { return m_calibrationInitialized; }

private:
    //! Sets the camera parameters based on the camera type.
    bool setCameraParameters(QString cameraType, camera_parameters& cameraParameters);

    //! Initializes the calibration.
    void calibrate(QString fileName);
    //! Camera is calibrated.
    bool m_calibrationInitialized;
    // members used for recalculation of world coordinates
    struct CalibrationPoint {
        double xWorld;
        double yWorld;
        double zWorld;
        double xImage;
        double yImage;
    };
    calibration_data m_calibrationData;
    camera_parameters m_cameraParameters;
    calibration_constants m_calibrationConstants;
};
#endif // CATS2_TRACKER_CALIBRATOR_HPP
