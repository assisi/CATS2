#include "CircularSetupController.hpp"
#include "settings/RobotControlSettings.hpp"
#include "FishBot.hpp"

#include <QtCore/QDebug>

CircularSetupController::CircularSetupController(FishBot* robot,
                                                 ExperimentControllerType::Enum type,
                                                 ExperimentControllerSettingsPtr settings):
    ExperimentController(robot, type),
    m_settings(),
    m_fishGroupTurningDirection(TurningDirection::UNDEFINED),
    m_previousFishAreaId(""),
    m_minAreaId(""),
    m_maxAreaId(""),
    m_targetTurningDirection(TurningDirection::UNDEFINED),
    m_setupCenter(PositionMeters::invalidPosition())
{
    CircularSetupControllerSettings* controllerSettings =
            dynamic_cast<CircularSetupControllerSettings*>(settings.data());
    if (controllerSettings != nullptr){
        // copy the parameters
        m_settings = controllerSettings->data();
    } else {
        qDebug() << "Could not set the controller's settings";
    }

    // load the control map
    readControlMap(m_settings.controlAreasFileName());

    // find the maximal area id
    QList<QString> areaIds = m_controlAreas.keys();
    if (areaIds.size() > 0) {
        m_maxAreaId = areaIds[0];
        m_minAreaId = areaIds[0];
        for (QString areaId : areaIds) {
            if (areaId > m_maxAreaId)
                m_maxAreaId = areaId;
            if (areaId < m_minAreaId)
                m_minAreaId = areaId;
        }
    }

    // find the setup center
    if (RobotControlSettings::get().setupMap().isValid())
        m_setupCenter = RobotControlSettings::get().setupMap().polygon().center();
}

/*!
 * Calculates the turning directions of the fish group.
 */
void CircularSetupController::computeFishTurningDirection()
{
    // remember where the fish were
    m_previousFishAreaId = m_fishAreaId;

    // find where the robot and the fish are now
    updateAreasOccupation();

    // compute the fish group turning direction
    if (m_controlAreas.contains(m_fishAreaId) &&
            m_controlAreas.contains(m_previousFishAreaId)) {
        int comparison = compareAreaIds(m_fishAreaId, m_previousFishAreaId);
        if (comparison > 0)
            m_fishGroupTurningDirection = TurningDirection::CLOCK_WISE;
        else if (comparison < 0)
            m_fishGroupTurningDirection = TurningDirection::COUNTER_CLOCK_WISE;
    }
}

/*!
 * Compares two area ids, returns 1 if first is bigger, -1 if it is smaller
 * and 0 if they are the same.
 */
int CircularSetupController::compareAreaIds(QString areaId1, QString areaId2)
{
    // the border case: the first id is bigger than the last id (circular setup)
    if ((areaId1 == m_maxAreaId) && (areaId2 == m_minAreaId))
        return -1;
    if ((areaId1 == m_minAreaId) && (areaId2 == m_maxAreaId))
        return 1;

    // other cases
    if (areaId1 == areaId2)
        return 0;
    else if (areaId1 > areaId2)
        return 1;
    else
        return -1;
}

/*!
 * Computes the target for the robot based on the target turning direction.
 */
PositionMeters CircularSetupController::computeTargetPosition()
{
    if (m_robot->state().position().isValid() &&
            (m_targetTurningDirection != TurningDirection::UNDEFINED))
    {
        double deltaAngleRad = m_settings.targetDeltaAngleRad();
        if (m_targetTurningDirection == TurningDirection::CLOCK_WISE)
            deltaAngleRad *= -1;
        return (m_robot->state().position().rotated2d(deltaAngleRad, m_setupCenter));
    } else {
        return PositionMeters::invalidPosition();
    }
}
