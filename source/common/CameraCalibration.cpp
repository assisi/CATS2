#include "CameraCalibration.hpp"
#include "settings/ReadSettingsHelper.hpp"
#include "AgentState.hpp"

/*!
 * Constructor. Gets the file name containing the camera calibration data.
 */
CameraCalibration::CameraCalibration(QString calibrationFileName, QSize targetFrameSize) :
    m_calibrationInitialized(false)
{
    calibrate(calibrationFileName, targetFrameSize);
}

/*!
 * Destructor.
 */
CameraCalibration::~CameraCalibration()
{

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

    // check first that the file exists
    if (!QFileInfo(calibrationFileName).exists()) {
        qDebug() << Q_FUNC_INFO << "The provided calibration file is not correct.";
        return;
    }

    // read the XML file
    ReadSettingsHelper settings(calibrationFileName);

    // read the camera type
    std::string cameraTypeString;
    settings.readVariable("cameraType", cameraTypeString);
    if (!setCameraParameters(QString::fromUtf8(cameraTypeString.c_str()), targetFrameSize, m_cameraParameters)) {
        // could not find camera parameters
        return;
    }

    // read the frame size that was used to set the calibration points
    // (can be different from the target frame size)
    // read the target image size
    int width;
    settings.readVariable(QString("imageSize/width"), width, -1); // default value to guarantee an
                                                                  // invalid size if the correct valus is not read
    int height;
    settings.readVariable(QString("imageSize/height"), height, -1); // default value to guarantee an
                                                                    // invalid size if the correct valus is not read
    QSize calibrationFrameSize(width, height);
    if (!calibrationFrameSize.isValid()) {
        qDebug() << Q_FUNC_INFO << "The calibration frame size is invalid.";
        return;
    }

    // read the units used for the calibration
    std::string units;
    int worldScaleCoefficient;
    settings.readVariable(QString("worldUnits"), units);
    if (units.empty()) {
        qDebug() << Q_FUNC_INFO << "Undefined the world units";
        return;
    } else {
        worldScaleCoefficient = getWorldScaleCoefficient(units);
        if (worldScaleCoefficient == 0)
            return;
    }

    // read the number of points
    int numberOfPoints;
    settings.readVariable("calibrationPoints/numberOfPoints", numberOfPoints, 0);
    if (numberOfPoints == 0) {
        qDebug() << Q_FUNC_INFO << "Couldn't read the number of points.";
        return;
    }

    // Put the points into the matrix
    if (numberOfPoints > TSAI_MAX_POINTS) {
        qDebug() << Q_FUNC_INFO << "There are more calibration points than accepted by the Tsai calibration library";
        return;
    }

    float imageScaleCoefficientX = (float)targetFrameSize.width() / (float)calibrationFrameSize.width();
    float imageScaleCoefficientY = (float)targetFrameSize.height() / (float)calibrationFrameSize.height();
    m_calibrationData.point_count = numberOfPoints;
    for(size_t i = 0; i < numberOfPoints; i++) {
        m_calibrationData.zw[i] = 0;
        settings.readVariable(QString("calibrationPoints/point_%1/xWorld").arg(i), m_calibrationData.xw[i]);
        m_calibrationData.xw[i] *= worldScaleCoefficient;
        settings.readVariable(QString("calibrationPoints/point_%2/yWorld").arg(i), m_calibrationData.yw[i]);
        m_calibrationData.yw[i] *= worldScaleCoefficient;
        settings.readVariable(QString("calibrationPoints/point_%1/xImage").arg(i), m_calibrationData.Xf[i]);
        m_calibrationData.Xf[i] *= imageScaleCoefficientX;
        settings.readVariable(QString("calibrationPoints/point_%2/yImage").arg(i), m_calibrationData.Yf[i]);
        m_calibrationData.Yf[i] *= imageScaleCoefficientY;
    }

    // Do the calibration
    try {
        m_calibrationInitialized = false;
        //noncoplanar_calibration_with_full_optimization(&calibrationData,&calibrationConstants,&cameraParameters);
        coplanar_calibration_with_full_optimization(&m_calibrationData, &m_calibrationConstants, &m_cameraParameters);
    }
    catch (...) {
        return;
    }

    //return true;
    m_calibrationInitialized = true;
}

/*!
 * Converts the position in pixels to the position in meters. The z is considered equal to zero.
 */
PositionMeters CameraCalibration::image2World(PositionPixels imageCoordinates)
{
    double wz = 0;
    double wx,wy;
    image_coord_to_world_coord(m_calibrationConstants, m_cameraParameters,
                               imageCoordinates.x(), imageCoordinates.y(),
                               wz, &wx, &wy);
    PositionMeters worldCoordinates(wx / 1000., wy / 1000.); // the calibration data is set in mm, hence the division
    return worldCoordinates;
}

/*!
 * Converts the position in meters to the position in pixels.
 */
PositionPixels CameraCalibration::world2Image(PositionMeters worldCoordinates)
{
    double ix, iy;
    world_coord_to_image_coord(m_calibrationConstants, m_cameraParameters,
                               worldCoordinates.x() * 1000, worldCoordinates.y() * 1000,
                               worldCoordinates.z() * 1000, &ix, &iy); // the calibration data is set in mm, hence the division
    return PositionPixels(ix, iy);
}

/*!
 * Converts the orientation at given position from image to world.
 */
OrientationRad CameraCalibration::image2WorldOrientationRad(PositionPixels imageCoordinates, OrientationRad imageOrientationRad)
{
    double vectorLength = m_cameraParameters.Ncx / 20; // empirical constant
    double ix = imageCoordinates.x() + vectorLength * cos(imageOrientationRad.angle()); // a vector in  image coordinates
    double iy = imageCoordinates.y() + vectorLength * sin(imageOrientationRad.angle());
    double wz = 0;
    double wx,wy;						// corresponding world coordinates
    double wox,woy;						// world coordinates of agent's position

    image_coord_to_world_coord(m_calibrationConstants, m_cameraParameters, ix, iy, wz, &wx, &wy);
    image_coord_to_world_coord(m_calibrationConstants, m_cameraParameters, imageCoordinates.x(), imageCoordinates.y(), wz, &wox, &woy);

    return OrientationRad(atan2(wy-woy, wx-wox));
}

/*!
 * Sets the camera parameters based on the camera type. Returns true if settings are set.
 */
bool CameraCalibration::setCameraParameters(QString cameraType, QSize frameSize, camera_parameters& cameraParameters)
{
    // FIXME TODO : get the correct values for the Chinese camera and for the Basler camera
    if (cameraType.toLower() == "c930e"){
        /* Given the ratio of dpx to dpy, simply pick some value for dpy, say 10um (or
            if you know it you can use the actual vertical pixel pitch) and use that to
            back calculate dpx, dx, and dy. Set Ncx = Nfx, sx = 1.0, and Cx and Cy to
            be the center of the frame buffer.  When you calibrate the model the
            algorithm will adjust sx, Cx, and Cy to give a best fit set of intrinsic
            parameters. (http://imagelab.ing.unimore.it/visor_test/faq_calibration.txt)*/
        double width = 4.8;
        double height = 3.6;
        double resX = frameSize.width();
        double resY = frameSize.height();
        cameraParameters.Ncx = resX;
        cameraParameters.Nfx = resX;
        cameraParameters.Cx = cameraParameters.Ncx / 2.;
        cameraParameters.Cy = resY / 2.;
        cameraParameters.dx = width / resX;
        cameraParameters.dpx = cameraParameters.dx * cameraParameters.Ncx / cameraParameters.Nfx;
        cameraParameters.dy = height / resY;
        cameraParameters.dpy = cameraParameters.dy;
        cameraParameters.sx = 1.0;
        return true;
    }
    if (cameraType.toLower() == "basler"){
        double resX = frameSize.width();
        double resY = frameSize.height();
        cameraParameters.Ncx = resX;
        cameraParameters.Nfx = resX;
        cameraParameters.Cx = cameraParameters.Ncx / 2.;
        cameraParameters.Cy = resY / 2.;
        cameraParameters.dx = 0.003;
        cameraParameters.dpx = cameraParameters.dx * cameraParameters.Ncx / cameraParameters.Nfx;
        cameraParameters.dy = 0.003;
        cameraParameters.dpy = cameraParameters.dy;
        cameraParameters.sx = 1.0;
        return true;
    }
    qDebug() << Q_FUNC_INFO << "Unknown camera type " << cameraType;
    return false;
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
