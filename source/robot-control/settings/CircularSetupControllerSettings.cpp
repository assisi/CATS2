#include "CircularSetupControllerSettings.hpp"

#include <settings/ReadSettingsHelper.hpp>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

/*!
 * Constructor.
 */
CircularSetupControllerSettings::CircularSetupControllerSettings(ExperimentControllerType::Enum type):
    ExperimentControllerSettings(type)
{

}

/*!
 * Destructor.
 */
CircularSetupControllerSettings::~CircularSetupControllerSettings()
{
    qDebug() << "Destroying the object";
}

/*!
 * Initialization of the parameters for this specific controller.
 */
bool CircularSetupControllerSettings::init(QString configurationFileName)
{
    // get the path of the configuration file
    QString configurationFolder = QFileInfo(configurationFileName).path();

    ReadSettingsHelper settings(configurationFileName);

    // if this controller is not set in settings then stop here
    if (!settings.validPath(m_settingPathPrefix))
        return false;

    // read the path to the control areas
    std::string controlAreasFilePath = "";
    settings.readVariable(QString("%1/controlAreasPath").arg(m_settingPathPrefix),
                          controlAreasFilePath, controlAreasFilePath);
    m_data.setControlAreasFileName(configurationFolder +
                                   QDir::separator() +
                                   QString::fromStdString(controlAreasFilePath));
    // read the target position radius
    double targetRadius = 0;
    settings.readVariable(QString("%1/targetPositionRadius").arg(m_settingPathPrefix),
                          targetRadius, targetRadius);
    m_data.setTargetRadiusM(targetRadius);
    // read the delta angle
    double angleDeg = 0;
    settings.readVariable(QString("%1/targetDeltaAngleDeg").arg(m_settingPathPrefix),
                          angleDeg, angleDeg);
    m_data.setTargetDeltaAngleRad(angleDeg * M_PI / 180.);

    // read trajectory
    std::string relativeTrajectoryPath = "";
    settings.readVariable(QString("%1/trajectoryFileName").arg(m_settingPathPrefix),
                          relativeTrajectoryPath,
                          relativeTrajectoryPath);
    m_data.setTrajectoryFileName(QString::fromStdString(relativeTrajectoryPath));
    QString trajectoryPath = configurationFolder +
                                QDir::separator() +
                                QString::fromStdString(relativeTrajectoryPath);
    QList<PositionMeters> trajectory;
    QFileInfo trajectoryFile(trajectoryPath);
    if (trajectoryFile.exists() && trajectoryFile.isFile()) {
        ReadSettingsHelper trajectorySettings(trajectoryPath);
        std::vector<cv::Point2f> polygon;
        trajectorySettings.readVariable(QString("polygon"), polygon);
        if (polygon.size() > 0) {
            for (auto& point : polygon)
                trajectory << PositionMeters(point);
            m_data.setTrajectory(trajectory);
            qDebug() << QString("Loaded trajectory of %1 points, used in circular setup"
                                "control modes for all robots").arg(trajectory.size());
        } else {
            qDebug() << "The trajectory is empty";
        }
    } else {
        qDebug() << "Could not find the trajectory file used in circular setup control modes";
    }

    // read trajectory current index
    int trajectoryCurrentIndex = 0;
    settings.readVariable(QString("%1/trajectoryCurrentIndex").arg(m_settingPathPrefix),
                          trajectoryCurrentIndex,
                          trajectoryCurrentIndex);
    if(trajectory.size() && trajectoryCurrentIndex >= trajectory.size()) {
        qDebug() << "Trajectory current index out of bounds";
        trajectoryCurrentIndex = 0;
    }
    m_data.setTrajectoryCurrentIndex(trajectoryCurrentIndex);


    return (QFileInfo(m_data.controlAreasFileName()).exists());
}
