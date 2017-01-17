#include "ExperimentController.hpp"

#include <settings/ReadSettingsHelper.hpp>

/*!
 * Constructor. Gets the file name containing the control areas description.
 */
ExperimentController::ExperimentController(FishBot* robot,                                           
                                           QString controlAreasFileName,
                                           ExperimentControllerType::Enum type) :
    QObject(nullptr),
    m_robot(robot),
    m_type(type)
{
    m_valid = deserialize(controlAreasFileName);
    if (m_valid)
        qDebug() << Q_FUNC_INFO << "Successfully read a control map from" << controlAreasFileName;
    else
        qDebug() << Q_FUNC_INFO << "Could not read a control map from" << controlAreasFileName;
}

/*!
 * Returns the control values for given position.
 */
ExperimentController::ControlData ExperimentController::step()
{
    return ControlData();
}

/*!
 * Reads the control map from a file.
 */
bool ExperimentController::deserialize(QString controlAreasFileName)
{
    bool successful = true;

    ReadSettingsHelper settings(controlAreasFileName);

    // read the number of areas
    int numberOfAreas;
    settings.readVariable("numberOfAreas", numberOfAreas);
    successful = successful && (numberOfAreas > 0);

    // read settings for every area.
    for (int areaIndex = 1; areaIndex <= numberOfAreas; areaIndex++) {
        // read area id
        std::string id;
        settings.readVariable(QString("area_%1/id").arg(areaIndex), id);
        // read the area type
        std::string type;
        settings.readVariable(QString("area_%1/type").arg(areaIndex), type);

        // create the area object
        ControlArea area(QString::fromUtf8(id.c_str()),
                         ControlAreaType::fromSettingsString(QString::fromUtf8(type.c_str())));

        // read polygons
        int numberOfPolygons;
        settings.readVariable(QString("area_%1/polygons/numberOfPolygons").arg(areaIndex), numberOfPolygons);
        for (int polygonIndex = 1; polygonIndex <= numberOfPolygons; polygonIndex++) {
            std::vector<cv::Point2f> polygon;
            settings.readVariable(QString("area_%1/polygons/polygon_%2").arg(areaIndex).arg(polygonIndex), polygon);
            area.addPolygon(polygon);
        }

        // read color
        int red;
        settings.readVariable(QString("area_%1/color/r").arg(areaIndex), red);
        int green;
        settings.readVariable(QString("area_%1/color/g").arg(areaIndex), green);
        int blue;
        settings.readVariable(QString("area_%1/color/b").arg(areaIndex), blue);
        area.setColor(QColor(red, green, blue));

        // read control mode
        std::string controlModeName;
        settings.readVariable(QString("area_%1/controlMode").arg(areaIndex), controlModeName);
        area.setControlMode(ControlModeType::fromSettingsString(QString::fromUtf8(controlModeName.c_str())));

        // read motion pattern
        std::string motionPatternName;
        settings.readVariable(QString("area_%1/motionPattern").arg(areaIndex), motionPatternName);
        area.setMotionPattern(MotionPatternType::fromSettingsString(QString::fromUtf8(motionPatternName.c_str())));

        m_controlAreas.append(area);
    }

    return successful;
}

/*!
 * Sends the area polygons.
 */
void ExperimentController::requestPolygons()
{
    QList<AnnotatedPolygons> polygons;
    for (const auto& area : m_controlAreas) {
        polygons.append(area.annotatedPolygons());
    }
    emit notifyPolygons(polygons);
}
