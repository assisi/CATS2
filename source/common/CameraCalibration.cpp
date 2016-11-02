#include "CameraCalibration.hpp"
#include "settings/ReadSettingsHelper.hpp"
#include "AgentState.hpp"

/*!
 * Constructor. Gets the file name containing the camera calibration data.
 */
CameraCalibration::CameraCalibration(QString calibrationFileName, QSize targetFrameSize) :
    m_agentHeight(0.),
    m_calibrationInitialized(false)
{
    if (readParameters(calibrationFileName))
        calibrate(calibrationFileName, targetFrameSize);
}

/*!
 * Destructor.
 */
CameraCalibration::~CameraCalibration()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
}

/*!
 * Initializes the calibration, compute coefficients
 */
void CameraCalibration::calibrate(QString calibrationFileName, QSize targetFrameSize)
{
    // check that the requested frame size is valid
    if (!targetFrameSize.isValid()) {
        qDebug() << Q_FUNC_INFO << "The requested target frame size is invalid.";
        return;
    }

    // check that the target size is compatible with the calibration size
    // meaning that the target size must be a scale of the calibration size
    m_imageScaleCoefficientX = static_cast<double>(targetFrameSize.width()) / static_cast<double>(m_calibrationFrameSize.width());
    m_imageScaleCoefficientY = static_cast<double>(targetFrameSize.height()) / static_cast<double>(m_calibrationFrameSize.height());
    if (! qFuzzyCompare(m_imageScaleCoefficientX, m_imageScaleCoefficientY)) {
        qDebug() << Q_FUNC_INFO << "The requested target frame size is not compatible with the size used to generate the calibration data.";
        return;
    }

    // read the XML file
    ReadSettingsHelper settings(calibrationFileName);

    // read the number of points
    int numberOfPoints;
    settings.readVariable("calibrationPoints/numberOfPoints", numberOfPoints, 0);
    if (numberOfPoints == 0) {
        qDebug() << Q_FUNC_INFO << "Couldn't read the number of points.";
        return;
    }

    // calibration points
    std::vector<cv::Point3f> worldPoints;
    std::vector<cv::Point2f> imagePoints;

    for (size_t i = 0; i < numberOfPoints; i++) {
        float zw = m_cameraHeight; // i.e. the camera is at z = 0
        float xw;
        settings.readVariable(QString("calibrationPoints/point_%1/xWorld").arg(i), xw);
        xw *= m_worldScaleCoefficient;
        float yw;
        settings.readVariable(QString("calibrationPoints/point_%2/yWorld").arg(i), yw);
        yw *= m_worldScaleCoefficient;
        worldPoints.push_back(cv::Point3f(m_xInversionCoefficient * xw,
                                          m_yInversionCoefficient * yw, zw));
        float xi;
        settings.readVariable(QString("calibrationPoints/point_%1/xImage").arg(i), xi);
        float yi;
        settings.readVariable(QString("calibrationPoints/point_%2/yImage").arg(i), yi);
        imagePoints.push_back(cv::Point2f(xi, yi));
    }

    // undistort the calibration points
    std::vector<cv::Point2f> undistortedImagePoints;
    cv::undistortPoints(imagePoints, undistortedImagePoints, m_cameraMatrix, m_distortionCoefficients, cv::Mat(), m_optimalCameraMatrix);

    // set the initial values for translation and rotation
    // we suppose the camera to be in the origin and no rotation
    m_rvec = cv::Mat::zeros(3, 1, cv::DataType<double>::type);
    m_tvec = cv::Mat::zeros(3, 1, cv::DataType<double>::type);

    // run the calibration on the undistorted points
    double rms = cv::solvePnP(worldPoints, undistortedImagePoints, m_optimalCameraMatrix, cv::Mat(), m_rvec, m_tvec, true,  cv::SOLVEPNP_EPNP);
//    qDebug() << Q_FUNC_INFO << "Calibrarion error" << rms;

    std::cout << m_rvec << std::endl;
    std::cout << m_tvec << std::endl;

    // get the rotation matrix
    cv::Mat rotationMatrix(3, 3, cv::DataType<double>::type);
    cv::Rodrigues(m_rvec, rotationMatrix);

    // uncomment to compute and print the reprojection error
    std::vector<cv::Point2f> projectedPoints;
    cv::projectPoints(worldPoints, m_rvec, m_tvec, m_optimalCameraMatrix, cv::Mat(), projectedPoints);
    double error = 0;
    for(unsigned int i = 0; i < projectedPoints.size(); ++i)
    {
        qDebug() << Q_FUNC_INFO
                 << QString("World point (%1, %2, %3)mm (corresponds (%4, %5)px) is projected to (%6, %7)px; error is %8 px")
                    .arg(worldPoints[i].x)
                    .arg(worldPoints[i].y)
                    .arg(worldPoints[i].z)
                    .arg(undistortedImagePoints[i].x)
                    .arg(undistortedImagePoints[i].y)
                    .arg(projectedPoints[i].x)
                    .arg(projectedPoints[i].y)
                    .arg(cv::norm(undistortedImagePoints[i] - projectedPoints[i]));
        error += cv::norm(undistortedImagePoints[i] - projectedPoints[i]);
    }
    error /= projectedPoints.size();
    qDebug() << Q_FUNC_INFO  << QString("Average projection error (world->image) is %1 pixels").arg(error);


    // TODO : to add an analysis on the error to decide if the calibration succeeded

    // compute all the stuff that is need to make the converions
    m_rotationMatrixInvCameraMatrixInv = rotationMatrix.inv() * m_optimalCameraMatrix.inv();
    m_rotationMatrixInvTranslationVector = rotationMatrix.inv() * m_tvec;

    // image to world error
    error = 0;
    for (int i = 0; i < imagePoints.size(); i++) {
        PositionMeters projectedPositionMeters = imageToWorld(PositionPixels(imagePoints.at(i)));
        PositionMeters originalPositionMeters(m_xInversionCoefficient * worldPoints.at(i).x / 1000.,
                                              m_yInversionCoefficient * worldPoints.at(i).y / 1000.,
                                              worldPoints.at(i).z / 1000);
        qDebug() << QString("%1 becomes %2, error is %3 mm")
                    .arg(originalPositionMeters.toString())
                    .arg(projectedPositionMeters.toString())
                    .arg(1000 * originalPositionMeters.distance2DTo(projectedPositionMeters));
        error += originalPositionMeters.distance2DTo(projectedPositionMeters);
    }
    error /= imagePoints.size();
    qDebug() << Q_FUNC_INFO  << QString("Average reprojection error (image->world) is %1 mm").arg(1000 * error);

    //return true;
    m_calibrationInitialized = true;
}

/*!
 * Converts the position in pixels to the position in meters. The z is considered equal to zero.
 */
PositionMeters CameraCalibration::imageToWorld(PositionPixels imageCoordinates)
{
    // find the undistorted image position
    std::vector<cv::Point2f> imagePoint;
    // we scale the image point to correspond to the calibration data
    imagePoint.push_back(cv::Point2f(imageCoordinates.x() / m_imageScaleCoefficientX,
                                     imageCoordinates.y() / m_imageScaleCoefficientY));
    std::vector<cv::Point2f> undistortedImagePoint;
    cv::undistortPoints(imagePoint, undistortedImagePoint,
                        m_cameraMatrix, m_distortionCoefficients,
                        cv::Mat(), m_optimalCameraMatrix);

    cv::Mat uvPoint = cv::Mat::ones(3, 1, cv::DataType<double>::type); //u,v,1
    uvPoint.at<double>(0,0) = undistortedImagePoint.at(0).x;
    uvPoint.at<double>(1,0) = undistortedImagePoint.at(0).y;

    // s * a = [wx wy wz]' + b, where a = R^{-1}*M^{-1}*[u v 1]', b = R^{-1}*t
    cv::Mat vectorA = m_rotationMatrixInvCameraMatrixInv * uvPoint;
    // TODO : to check if it works better when the height of the agent is taken into account
    double wz = m_cameraHeight /*- m_agentHeight*/;
    double s = wz + m_rotationMatrixInvTranslationVector.at<double>(2, 0);
    s /= vectorA.at<double>(2, 0);

    double wx = s * vectorA.at<double>(0, 0) - m_rotationMatrixInvTranslationVector.at<double>(0, 0);
    double wy = s * vectorA.at<double>(1, 0) - m_rotationMatrixInvTranslationVector.at<double>(1, 0);

    // NOTE : the Z-coordinate is not set is we work in 2D world
    PositionMeters worldCoordinates(m_xInversionCoefficient * wx / 1000., m_yInversionCoefficient * wy / 1000.); // the calibration data is set in mm, hence the division

    return worldCoordinates;
}

/*!
 * Converts the position in meters to the position in pixels.
 */
PositionPixels CameraCalibration::worldToImage(PositionMeters worldCoordinates)
{
    std::vector<cv::Point2f> projectedPoint;
    std::vector<cv::Point3f> worldPoint;
    // HACK : since only "x" and "y" are sinchronized between the top and bottom setus,
    // the "z" value is to be set  to the "agent height" specific for this setup
    // TODO : to check if it works better when the height of the agent is taken into account
    worldCoordinates.setZ(m_cameraHeight /*- m_agentHeight*/);
    // the calibration data is set in mm, hence the multiplication to change from meters
    worldPoint.push_back(cv::Point3f(m_xInversionCoefficient * worldCoordinates.x() * 1000,
                                     m_yInversionCoefficient * worldCoordinates.y() * 1000,
                                     worldCoordinates.z()/* * 1000*/));
    cv::projectPoints(worldPoint, m_rvec, m_tvec, m_cameraMatrix, m_distortionCoefficients, projectedPoint);
    return PositionPixels(projectedPoint[0].x, projectedPoint[0].y);
}

/*!
 * Converts the orientation at given position from image to world.
 */
OrientationRad CameraCalibration::imageToWorldOrientationRad(PositionPixels imageCoordinates, OrientationRad imageOrientationRad)
{
    double vectorLength = 50; // px, empirical constant
    double u = imageCoordinates.x() + vectorLength * cos(imageOrientationRad.angle()); // a vector in  image coordinates
    double v = imageCoordinates.y() + vectorLength * sin(imageOrientationRad.angle());
    PositionPixels imageEndPoint(u, v);

    PositionMeters worldCoordinates = imageToWorld(imageCoordinates);
    PositionMeters worldEndPoint = imageToWorld(imageEndPoint);

    return OrientationRad(atan2(worldEndPoint.y() - worldCoordinates.y(),
                                worldEndPoint.x() - worldCoordinates.x()));
}

/*!
 * Converts the orientation at given position from world to image.
 */
OrientationRad CameraCalibration::worldToImageOrientationRad(PositionMeters worldCoordinates, OrientationRad worldOrientationRad)
{
    double vectorLengthMm = 100; // mm, empirical constant
    double wx = worldCoordinates.x() * 1000 + vectorLengthMm * cos(worldOrientationRad.angle()); // a vector in  image coordinates
    double wy = worldCoordinates.y() * 1000 + vectorLengthMm * sin(worldOrientationRad.angle());

    PositionMeters worldEndPoint(wx, wy, worldCoordinates.z());

    PositionPixels imageCoordinates = worldToImage(worldCoordinates);
    PositionPixels imageEndPoint = worldToImage(worldEndPoint);

    return OrientationRad(atan2(imageEndPoint.y() - imageCoordinates.y(),
                                imageEndPoint.x() - imageCoordinates.x()));
}

/*!
 * Computes the linear coefficient to convert the calibratin
 * values to mm based on the present data units. Returns 0 if the units are not known / supported.
 */
int CameraCalibration::getWorldScaleCoefficient(std::string units)
{
    float coefficient = 0;
    if (units.compare("mm") == 0) { // 0 means that the strings coinside
        coefficient = 1;
    } else if (units.compare("cm") == 0) {
        coefficient = 10;
    } else if (units.compare("m") == 0) {
        coefficient = 100;
    } else {
        qDebug() << Q_FUNC_INFO << "Unknown world units " << units.data();
    }
    return coefficient;
}

/*!
 * Read parameters for the calibration procedure. Returns true if all necessary
 * data is there.
 */
bool CameraCalibration::readParameters(QString calibrationFileName)
{
    // check first that the file exists
    if (!QFileInfo(calibrationFileName).exists()) {
        qDebug() << Q_FUNC_INFO << "The provided calibration file is not correct.";
        return false;
    }

    // read the XML file
    ReadSettingsHelper settings(calibrationFileName);

    // read the frame size that was used to set the calibration points
    // (can be different from the target frame size)
    // read the target image size
    int width;
    settings.readVariable(QString("imageSize/width"), width, -1); // default value to guarantee an
                                                                  // invalid size if the correct valus is not read
    int height;
    settings.readVariable(QString("imageSize/height"), height, -1); // default value to guarantee an
                                                                    // invalid size if the correct valus is not read
    m_calibrationFrameSize = QSize(width, height);
    if (!m_calibrationFrameSize.isValid()) {
        qDebug() << Q_FUNC_INFO << "The calibration frame size is invalid.";
        return false;
    }

    // read the units used for the calibration
    std::string units;
    m_worldScaleCoefficient;
    settings.readVariable(QString("worldUnits"), units);
    if (units.empty()) {
        qDebug() << Q_FUNC_INFO << "Undefined the world units";
        return false;
    } else {
        m_worldScaleCoefficient = getWorldScaleCoefficient(units);
        if (m_worldScaleCoefficient == 0)
            return false;
    }

    // read the agents' altitude
    settings.readVariable(QString("agentHeight"), m_agentHeight, 0.);
    m_agentHeight *= m_worldScaleCoefficient; // in mm

    // read the camera's height
    settings.readVariable(QString("cameraHeight"), m_cameraHeight, 0.);
    m_cameraHeight *= m_worldScaleCoefficient; // in mm

    // read the inversion flags
    bool invertY;
    settings.readVariable(QString("invertY"), invertY, false);
    if (invertY)
        m_yInversionCoefficient = -1;
    else
        m_yInversionCoefficient = 1;

    bool invertX;
    settings.readVariable(QString("invertX"), invertX, false);
    if (invertX)
        m_xInversionCoefficient = -1;
    else
        m_xInversionCoefficient = 1;

    // read the camera matrix
    settings.readVariable("cameraMatrix", m_cameraMatrix);
    std::cout << m_cameraMatrix << std::endl;

    // read the distortion coefficients
    settings.readVariable("distortionCoefficients", m_distortionCoefficients);
    std::cout << m_distortionCoefficients << std::endl;

    // compute the optimal new camera matrix
    m_optimalCameraMatrix = cv::getOptimalNewCameraMatrix(m_cameraMatrix, m_distortionCoefficients, cv::Size(width, height), 1.0);

    return true;
}
