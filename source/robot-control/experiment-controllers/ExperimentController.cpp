#include "ExperimentController.hpp"

#include "ControlArea.hpp"

#include <settings/ReadSettingsHelper.hpp>

#include <QtCore/QFileInfo>

/*!
 * Constructor. Gets the file name containing the control areas description.
 */
ExperimentController::ExperimentController(FishBot* robot,
                                           ExperimentControllerType::Enum type) :
    QObject(nullptr),
    m_robot(robot),
    m_type(type)
{

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
void ExperimentController::readControlMap(QString controlAreasFileName)
{
    ReadSettingsHelper settings(controlAreasFileName);

    // read the number of areas
    int numberOfAreas;
    settings.readVariable("numberOfAreas", numberOfAreas);

    // read settings for every area.
    for (int areaIndex = 1; areaIndex <= numberOfAreas; areaIndex++) {
        // read area id
        std::string id;
        settings.readVariable(QString("area_%1/id").arg(areaIndex), id);
        // read the area type
        std::string type;
        settings.readVariable(QString("area_%1/type").arg(areaIndex), type);

        // create the area object
        ControlAreaType::Enum areaType =
                ControlAreaType::fromSettingsString(QString::fromStdString(type.c_str()));
        ControlAreaPtr area(new ControlArea(QString::fromStdString(id.c_str()),
                                            areaType));

        // read polygons
        int numberOfPolygons;
        settings.readVariable(QString("area_%1/polygons/numberOfPolygons")
                              .arg(areaIndex), numberOfPolygons);
        for (int polygonIndex = 1; polygonIndex <= numberOfPolygons; polygonIndex++) {
            std::vector<cv::Point2f> polygon;
            settings.readVariable(QString("area_%1/polygons/polygon_%2")
                                  .arg(areaIndex).arg(polygonIndex), polygon);
            area->addPolygon(polygon);
        }

        // read the area color (for gui)
        int red;
        settings.readVariable(QString("area_%1/color/r").arg(areaIndex), red);
        int green;
        settings.readVariable(QString("area_%1/color/g").arg(areaIndex), green);
        int blue;
        settings.readVariable(QString("area_%1/color/b").arg(areaIndex), blue);
        area->setColor(QColor(red, green, blue));

        // read control mode
        std::string controlModeName;
        settings.readVariable(QString("area_%1/controlMode").arg(areaIndex), controlModeName);
        area->setControlMode(ControlModeType::fromSettingsString(QString::fromStdString(controlModeName.c_str())));

        // read motion pattern
        std::string motionPatternName;
        settings.readVariable(QString("area_%1/motionPattern").arg(areaIndex), motionPatternName);
        area->setMotionPattern(MotionPatternType::fromSettingsString(QString::fromStdString(motionPatternName.c_str())));

        // add to the areas map
        m_controlAreas[area->id()] = area;
    }

    // read the prefered area if available
    std::string preferedAreaId;
    settings.readVariable(QString("preferedAreaId"), preferedAreaId);
    m_preferedAreaId = QString::fromStdString(preferedAreaId.c_str());
}

/*!
 * Sends the area polygons.
 */
void ExperimentController::requestPolygons()
{
    QList<AnnotatedPolygons> polygons;
    for (const auto area : m_controlAreas.values()) {
        polygons.append(area->annotatedPolygons());
    }
    emit notifyPolygons(polygons);
}
