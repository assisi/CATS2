#include "CircularSetupController.hpp"
#include "settings/RobotControlSettings.hpp"
#include "FishBot.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor. Gets the robot and the settings.
 */
CircularSetupController::CircularSetupController(FishBot* robot,
                                                 ExperimentControllerSettingsPtr settings,
                                                 ExperimentControllerType::Enum controllerType):
    ExperimentController(robot, controllerType),
    m_settings(),
    m_fishGroupTurningDirection(TurningDirection::UNDEFINED),
    m_previousFishAreaId(""),
    m_minAreaId(""),
    m_maxAreaId(""),
    m_targetTurningDirection(TurningDirection::UNDEFINED),
    m_setupCenter(PositionMeters::invalidPosition()),
    m_robotClockWiseCounter(0),
    m_robotUndefCounter(0),
    m_fishClockWiseCounter(0),
    m_fishUndefCounter(0),
    m_allMeasurementsCounter(0)
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

    // connect the timer to the print method
    connect(&m_statisticsPrintTimer, &QTimer::timeout, [=](){ printStatistics(); });

    // connect the timer to the statistics publisher method
    connect(&m_statisticsPublisherTimer, &QTimer::timeout, [=](){
        double fishClockWisePercent = 1 * static_cast<double>(m_fishClockWiseCounter) /
                static_cast<double>(m_allMeasurementsCounter);
        double fishCounterClockWisePercent = 1 *
                static_cast<double>(m_allMeasurementsCounter -
                                    m_fishClockWiseCounter -
                                    m_fishUndefCounter) /
                static_cast<double>(m_allMeasurementsCounter);

        double robotClockWisePercent = 1 * static_cast<double>(m_robotClockWiseCounter) /
                static_cast<double>(m_allMeasurementsCounter);
        double robotCounterClockWisePercent = 1 *
                static_cast<double>(m_allMeasurementsCounter -
                                    m_robotClockWiseCounter -
                                    m_robotUndefCounter) /
                static_cast<double>(m_allMeasurementsCounter);
                emit notifyStatisticsAvailable(fishClockWisePercent, fishCounterClockWisePercent, robotClockWisePercent, robotCounterClockWisePercent);
	});
}

/*!
 * Destructor.
 */
CircularSetupController::~CircularSetupController()
{
    m_statisticsPrintTimer.stop();
    m_statisticsPublisherTimer.stop();
    bool finalCall = true;
    printStatistics(finalCall);
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
 * Update the turning directions statistics.
 */
void CircularSetupController::updateStatistics()
{
    m_allMeasurementsCounter++;

    if (m_fishGroupTurningDirection == TurningDirection::CLOCK_WISE)
        m_fishClockWiseCounter++;
    else if (m_fishGroupTurningDirection == TurningDirection::UNDEFINED)
        m_fishUndefCounter++;

    if (m_targetTurningDirection == TurningDirection::CLOCK_WISE)
        m_robotClockWiseCounter++;
    else if (m_targetTurningDirection == TurningDirection::UNDEFINED)
        m_robotUndefCounter++;
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
        double angleToRobotRad = m_setupCenter.angleRadTo(m_robot->state().position());
        return m_setupCenter.positionAt(angleToRobotRad + deltaAngleRad, m_settings.targetRadiusM());
    } else {
        return PositionMeters::invalidPosition();
    }
}

/*!
 * Updates the current target turning direction for the robot.
 */
bool CircularSetupController::updateTargetTurningDirection(TurningDirection::Enum turningDirection)
{
    // TODO put elsewhere, do it only if a parameter enables it
    // Reset statistics
    m_robotClockWiseCounter = 0;
    m_robotUndefCounter = 0;
    m_fishClockWiseCounter = 0;
    m_fishUndefCounter = 0;
    m_allMeasurementsCounter = 0;

    if (m_targetTurningDirection != turningDirection) {
        qDebug() << QString("%1 turning direction changed from %2 to %3")
                    .arg(m_robot->name())
                    .arg(TurningDirection::toString(m_targetTurningDirection))
                    .arg(TurningDirection::toString(turningDirection));
        m_targetTurningDirection = turningDirection;
        return true;
    } else
        return false;
}

/*!
 * Called when the controller is activated. Used to reset parameters.
 */
void CircularSetupController::start()
{
// NOTE : we collect the data for the whole program run-time
//    m_clockWiseCounter = 0;
//    m_allMeasurementsCounter = 0;
    // start the timer to print the circular setup statistics
    int stepMsec = 60000; // 1 minute
    m_statisticsPrintTimer.start(stepMsec);

    // TODO add a configuration parameter to set this
    int stepPublisherMsec = 1000; // 1 second
    m_statisticsPublisherTimer.start(stepMsec);
}

/*!
 * Called when the controller is disactivated.
 */
void CircularSetupController::finish()
{
    printStatistics();
    m_statisticsPrintTimer.stop();
    m_statisticsPublisherTimer.stop();
}

/*!
 * Prints the turning directions statistics.
 */
void CircularSetupController::printStatistics(bool final)
{
    if (m_allMeasurementsCounter != 0) {
        QString startingText = final ?
                    "Experiment is finished, final" : "Ongoing experiment";

        double fishClockWisePercent = 100 * static_cast<double>(m_fishClockWiseCounter) /
                static_cast<double>(m_allMeasurementsCounter);
        double fishCounterClockWisePercent = 100 *
                static_cast<double>(m_allMeasurementsCounter -
                                    m_fishClockWiseCounter -
                                    m_fishUndefCounter) /
                static_cast<double>(m_allMeasurementsCounter);

        double robotClockWisePercent = 100 * static_cast<double>(m_robotClockWiseCounter) /
                static_cast<double>(m_allMeasurementsCounter);
        double robotCounterClockWisePercent = 100 *
                static_cast<double>(m_allMeasurementsCounter -
                                    m_robotClockWiseCounter -
                                    m_robotUndefCounter) /
                static_cast<double>(m_allMeasurementsCounter);

        qDebug() << QString("%10 statistics: fish went clock-wise %1\% of time "
                            "(%2/%3)  and counter-clock-wise %4\% of time; the "
                            "robot %5 went clock-wise %6\% of time (%7/%8) and "
                            "counter-clock-wise %9\% of time")
                    .arg(fishClockWisePercent)
                    .arg(m_fishClockWiseCounter)
                    .arg(m_allMeasurementsCounter)
                    .arg(fishCounterClockWisePercent)
                    .arg(m_robot->name())
                    .arg(robotClockWisePercent)
                    .arg(m_robotClockWiseCounter)
                    .arg(m_allMeasurementsCounter)
                    .arg(robotCounterClockWisePercent)
                    .arg(startingText);
    } else {
        qDebug() << "Experiment is finished but no measurements were made";
    }
}
