#include "CameraCalibration.hpp"
#include "settings/ReadSettingsHelper.hpp"
#include "AgentState.hpp"

/*!
 * Constructor.
 */
CameraCalibration::CameraCalibration(QString calibrationFileName) :
    m_calibrationInitialized(false)
{
    calibrate(calibrationFileName);
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
void CameraCalibration::calibrate(QString calibrationFileName)
{
    // read the XML file
    ReadSettingsHelper settings(calibrationFileName);

    // read the camera type
    std::string cameraTypeString;
    settings.readVariable("cameraType", cameraTypeString);
    if (!setCameraParameters(QString::fromUtf8(cameraTypeString.c_str()), m_cameraParameters)) { // could not find camera parameters
        return;
    }

//    // read the calibration points
//    QList<CalibrationPoint> calibrationPointList;

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

    for(size_t i = 1; i <= numberOfPoints; i++) {
//        CalibrationPoint calibrationPoint;
        m_calibrationData.zw[i] = 0;
        settings.readVariable(QString("calibrationPoints/point_%1/xWorld").arg(i), m_calibrationData.xw[i]);
        settings.readVariable(QString("calibrationPoints/point_%2/xWorld").arg(i), m_calibrationData.yw[i]);
        settings.readVariable(QString("calibrationPoints/point_%1/xImage").arg(i), m_calibrationData.Xf[i]);
        settings.readVariable(QString("calibrationPoints/point_%2/yImage").arg(i), m_calibrationData.Yf[i]);
//        calibrationPointList.append(calibrationPoint);
    }

//    _calibrationData.point_count = calibrationPointList.size();
//    for (size_t i = 0; i < calibrationData.point_count; i++) {
//        _calibrationData.zw[i]=0;
//        _calibrationData.xw[i]=(calibrationPointList.at(i)).xWorld;
//        _calibrationData.yw[i]=(calibrationPointList.at(i)).yWorld;
//        _calibrationData.Xf[i]=(calibrationPointList.at(i)).xImage;
//        _calibrationData.Yf[i]=(calibrationPointList.at(i)).yImage;
//    }

//    //Getting camera parametrs from configuration file

//    _cameraParameters.Ncx = 1280; //1440;
//    _cameraParameters.Nfx = 1280;//1440;
//    _cameraParameters.dx = 0.0065;
//    _cameraParameters.dpx = _cameraParameters.dx * _cameraParameters.Ncx / _cameraParameters.Nfx;
//    _cameraParameters.dy = 0.0065;
//    _cameraParameters.dpy = cameraParameters.dy * 960/960; //1080/1080;
//    _cameraParameters.Cx = cameraParameters.Ncx/2;
//    _cameraParameters.Cy = 960/2; //1080/2;
//    _cameraParameters.sx = 1.0;


    // Do the calibration
    try {
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
    PositionMeters worldCoordinates(wx, wy);
    return worldCoordinates;
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
bool CameraCalibration::setCameraParameters(QString cameraType, camera_parameters& cameraParameters)
{
    if (cameraType.toLower() == "c930e"){
        /* Given the ratio of dpx to dpy, simply pick some value for dpy, say 10um (or
            if you know it you can use the actual vertical pixel pitch) and use that to
            back calculate dpx, dx, and dy. Set Ncx = Nfx, sx = 1.0, and Cx and Cy to
            be the center of the frame buffer.  When you calibrate the model the
            algorithm will adjust sx, Cx, and Cy to give a best fit set of intrinsic
            parameters. (http://imagelab.ing.unimore.it/visor_test/faq_calibration.txt)*/
        double width = 4.8;
        double height = 3.6;
        double resX = 1920;
        double resY = 1080;
        m_cameraParameters.Ncx = resX;
        m_cameraParameters.Nfx = resX;
        m_cameraParameters.Cx = cameraParameters.Ncx / 2.;
        m_cameraParameters.Cy = resY / 2.;
        m_cameraParameters.dx = width / resX;
        m_cameraParameters.dpx = m_cameraParameters.dx * m_cameraParameters.Ncx / m_cameraParameters.Nfx;
        m_cameraParameters.dy = height / resY;
        m_cameraParameters.dpy = cameraParameters.dy;
        m_cameraParameters.sx = 1.0;
        return true;
    }
    qDebug() << Q_FUNC_INFO << "Unknown camera type";
    return false;
}
