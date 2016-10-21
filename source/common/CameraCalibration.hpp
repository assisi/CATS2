#ifndef CATS2_TRACKER_CALIBRATOR_HPP
#define CATS2_TRACKER_CALIBRATOR_HPP


#include <opencv2/opencv.hpp>

//#include <libtsai.h>

#include <QtCore>
#include <QtGui>
#include <QtXml/QDomDocument>

class PositionPixels;
class PositionMeters;
class OrientationRad;

// TODO : needs unit testing.

/*!
 * \brief The CameraCalibration class serves to compute the camera's parameters that are
 * later used to convert between image and world coordinates.
  * Current implementation uses the OpenCV based calibration routine. The camera parameters
  * are computed wiht the help of the chessboard and then the calibration points are used
  * to synchronize both setups.
 */
class CameraCalibration
{
public:
    //! Constructor. Gets the file name containing the camera calibration data.
    explicit CameraCalibration(QString calibrationFileName, QSize targetFrameSize);
    //! Destructor.
    virtual ~CameraCalibration() final;

    //! Converts the position in pixels to the position in meters.
    PositionMeters imageToWorld(PositionPixels imageCoordinates);
    //! Converts the position in meters to the position in pixels.
    PositionPixels worldToImage(PositionMeters worldCoordinates);

    //! Converts the orientation at given position from image to world.
    //! FIXME : this method calls imageToWorldPosition() and so it is not optimal to call both methods, need to be merged somehow.
    OrientationRad imageToWorldOrientationRad(PositionPixels imageCoordinates, OrientationRad imageOrientationRad);
    //! Converts the orientation at given position from world to image.
    OrientationRad worldToImageOrientationRad(PositionMeters worldCoordinates, OrientationRad worldOrientationRad);

    bool isInitialized() const { return m_calibrationInitialized; }

private:
    //! Read parameters for the calibration procedure. Returns true if all
    //! necessary data is there.
    bool readParameters(QString calibrationFileName);
    //! Computes the linear coefficient to convert the calibratin values to mm based on the
    //! present data units. Returns 0 if the units are not known / supported.
    int getWorldScaleCoefficient(std::string units);
    //! The (average) altitude of the agents with respect to the calibration data.
    double m_agentHeight; // [mm]
    //! The distance between the setup and the camera
    double m_cameraHeight; // [mm]
    //! A coefficient that defines if the "x" image coordinate should be inverted.
    //! We try to adjust the axis so that there is minimum of rotation with respect
    //! to the fram coordinate system
    int m_xInversionCoefficient;
    //! A coefficient that defines if the "y" image coordinate should be inverted.
    //! We try to adjust the axis so that there is minimum of rotation with respect
    //! to the fram coordinate system
    int m_yInversionCoefficient;

    //! Initializes the calibration.
    void calibrate(QString fileName, QSize targetFrameSize);
    //! Camera is calibrated.
    bool m_calibrationInitialized;

    // NOTE : comes from http://docs.opencv.org/2.4/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html#solvepnp
    // and http://stackoverflow.com/questions/12299870/computing-x-y-coordinate-3d-from-image-point
    //! The coefficient to scale the calibration data to [mm].
    int m_worldScaleCoefficient;
    //! The size of the calibration data image.
    QSize m_calibrationFrameSize;
    //! The camera matrix.
    cv::Mat m_cameraMatrix;
    //! The distortion coefficients.
    cv::Mat m_distortionCoefficients;
    //! The matrix that would correspond to this camera without distortion.
    cv::Mat m_optimalCameraMatrix;
    //! Translation vector.
    cv::Mat m_rvec;
    //! Rotation vector.
    cv::Mat m_tvec;

    //! Auxiliary matrices.
    cv::Mat m_rotationMatrixInvCameraMatrixInv; // R^{-1}*M^{-1}
    cv::Mat m_rotationMatrixInvTranslationVector ; // R^{-1}*t

    //! Since the calibration is done for the calibration frame size, but we work in
    //! the target frame size, it need to be taken into account.
    double m_imageScaleCoefficientX;
    double m_imageScaleCoefficientY;
};
#endif // CATS2_TRACKER_CALIBRATOR_HPP
