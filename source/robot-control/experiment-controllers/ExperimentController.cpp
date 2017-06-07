#include "ExperimentController.hpp"
#include "FishBot.hpp"
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
    m_controlAreas(),
    m_preferedAreaId(""),
    m_fishAreaId(""),
    m_robotAreaId(""),
    m_robotAreaChanged(false),
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

/*!
 * Find where the robot and the fish are.
 */
void ExperimentController::updateAreasOccupation()
{
    QString areaId;
    if (findRobotArea(areaId)) {
        updateRobotArea(areaId);
    } else {
        // reset the variable
        m_robotAreaId = "";
    }
    if (findFishArea(areaId)) {
        if (m_fishAreaId != areaId) {
//            qDebug() << QString("Most of fish is now in room %1 (before %2) : %3")
//                        .arg(areaId)
//                        .arg(m_fishAreaId)
//                        .arg(m_fishNumberByArea[areaId]);
            m_fishAreaId = areaId;
        }
    } else {
        // reset the variable
        m_fishAreaId = "";
    }
}

/*!
 * Finds the room with the majority of fish. Returns the success status.
 */
bool ExperimentController::findFishArea(QString& maxFishNumberAreaId)
{
    bool status = false;
    if (m_robot) {
        // setup the map to count the fish
        for (const auto& areaId : m_controlAreas.keys())
            m_fishNumberByArea[areaId] = 0;
        // get the fish states
        QList<StateWorld> fishStates = m_robot->fishStates();
        if (fishStates.size() > 0) {
            // for every fish check where it is
            for (const auto& fishState : fishStates) {
                QString areaId;
                if (findAreaByPosition(areaId, fishState.position())) {
                    m_fishNumberByArea[areaId]++;
                }
            }
            // find the majority of fish
            QString prevMaxFishNumberAreaId = maxFishNumberAreaId; // backup
            int maxFishNumber = 0;
            for (const auto& areaId : m_controlAreas.keys()) {
                if (m_fishNumberByArea[areaId] > maxFishNumber) {
                    maxFishNumber = m_fishNumberByArea[areaId];
                    maxFishNumberAreaId = areaId;
                }
            }
            // restore the values if nothing found
            if (maxFishNumber == 0) {
                maxFishNumberAreaId = prevMaxFishNumberAreaId;
                status = false;
            } else {
                status = true;
            }
        }
    }
    // send the area's occupation data update
    emit notifyFishNumberByAreas(m_fishNumberByArea);
    // return the status
    return status;
}

/*!
 * Finds the room where the robot is. Returns the success status.
 */
bool ExperimentController::findRobotArea(QString& robotAreaId)
{
    if (m_robot) {
        return findAreaByPosition(robotAreaId, m_robot->state().position());
    }
    return false;
}

/*!
 * Finds the room that contains given point.
 */
bool ExperimentController::findAreaByPosition(QString& areaId,
                                              const PositionMeters& position)
{
    if (position.isValid()) {
        for (const auto controlArea : m_controlAreas.values()) {
            if (controlArea->contains(position)) {
                areaId = controlArea->id();
                return true;
            }
        }
    }
    return false;
}

/*!
 * Counts the fish number in all rooms different from the current one. We don't
 * take into account corridors.
 */
int ExperimentController::fishNumberInOtherRooms(QString currentAreaId)
{
    // count the fish
    int fishNumber = 0;
    for (const auto& areaId : m_controlAreas.keys()) {
        if ((areaId != currentAreaId) &&
                (m_controlAreas[areaId]->type() == ControlAreaType::ROOM))
            fishNumber += m_fishNumberByArea[areaId];
    }
    return fishNumber;
}

/*!
 * Sets the robot's area.
 */
void ExperimentController::updateRobotArea(QString areaId)
{
    if (areaId != m_robotAreaId) {
        m_robotAreaId = areaId;
        m_robotAreaChanged = true;
//            qDebug() << QString("%1 changed the room from %2 to %3")
//                        .arg(m_robot->name())
//                        .arg(m_robotAreaId)
//                        .arg(areaId);
    } else {
        m_robotAreaChanged = false;
    }
}
