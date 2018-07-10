#include "ExperimentManager.hpp"
#include "MapController.hpp"
#include "ExperimentControllerFactory.hpp"

#include "FishBot.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
ExperimentManager::ExperimentManager(FishBot* robot) :
    QObject(nullptr),
    m_currentController(ExperimentControllerType::NONE),
    m_robot(robot)
{
    // fill the map will all controllers
    // first we add an empty controller for the sake simplicity in the
    // setController() implementation
    m_controllers.insert(ExperimentControllerType::NONE,
                         ExperimentControllerPtr(new ExperimentController(m_robot,
                                                                          ExperimentControllerType::NONE)));

    // and then the rest of controllers available in the settings
    for (auto controllerType : RobotControlSettings::get().availableControllers())
    {
        m_controllers.insert(controllerType,
                             ExperimentControllerFactory::createController(controllerType,
                                                                           m_robot));
        // make connections
        connect(m_controllers[controllerType].data(),
                &ExperimentController::notifyFishNumberByAreas,
                this, &ExperimentManager::notifyFishNumberByAreas);
    }

    // make connections
    connect(m_controllers[m_currentController].data(),
            &ExperimentController::notifyPolygons,
            this, &ExperimentManager::notifyPolygons);
    connect(m_controllers[m_currentController].data(),
            &ExperimentController::notifyControllerStatus,
            this, &ExperimentManager::notifyControllerStatus);

    // connect experiment specific signals
    for (int controllerTypeInd = ExperimentControllerType::CIRCULAR_SETUP_FOLLOWER;
         controllerTypeInd <= ExperimentControllerType::CIRCULAR_SETUP_LEADER_CCW_MODEL;
         controllerTypeInd++)
    {
        ExperimentControllerType::Enum controllerType = static_cast<ExperimentControllerType::Enum>(controllerTypeInd);
        if (m_controllers.contains(controllerType)) {
            CircularSetupController* controller =
                    dynamic_cast<CircularSetupController*>(m_controllers[controllerType].data());
            if (controller) {
                connect(controller,
                        &CircularSetupController::notifyTurningDirections,
                        this,
                        &ExperimentManager::notifyCircularSetupTurningDirections);

                connect(controller,
                        &CircularSetupController::notifyStatisticsAvailable,
                        this,
                        &ExperimentManager::notifyCircularSetupStatistics);
            }
        }
    }
}

/*!
 * Sets the requested controller.
 */
void ExperimentManager::setController(ExperimentControllerType::Enum type)
{
    if (!m_controllers.contains(type)) {
        qDebug() << "Requested controller is not available";
        return;
    }

    if (type != m_currentController) {
        qDebug() << QString("Changing the controller from %1 to %2 for %3")
                    .arg(ExperimentControllerType::toString(m_currentController))
                    .arg(ExperimentControllerType::toString(type))
                    .arg(m_robot->name());

        // disable connections
        disconnect(m_controllers[m_currentController].data(),
                   &ExperimentController::notifyPolygons,
                   this, &ExperimentManager::notifyPolygons);
        disconnect(m_controllers[m_currentController].data(),
                &ExperimentController::notifyControllerStatus,
                this, &ExperimentManager::notifyControllerStatus);
        // first stop the current controller
        m_controllers[m_currentController]->finish();

        // then start the new one
        m_currentController = type;
        // inform about changes
        emit notifyControllerChanged(type);
        // add connections
        connect(m_controllers[m_currentController].data(),
                &ExperimentController::notifyPolygons,
                this, &ExperimentManager::notifyPolygons);
        connect(m_controllers[m_currentController].data(),
                &ExperimentController::notifyControllerStatus,
                this, &ExperimentManager::notifyControllerStatus);
        // prepare the controller
        m_controllers[m_currentController]->start();
        // requests the new controller's control areas polygons
        m_controllers[m_currentController]->requestPolygons();
    }
}

/*!
 * The experiment manager step. Returns the control target values.
 */
ExperimentController::ControlData ExperimentManager::step()
{
    return m_controllers[m_currentController]->step();
}

/*!
 * Sets the circular setup robot turning direction (CW/CCW).
 */
void ExperimentManager::setCircularSetupTurningDirection(QString message)
{
    qDebug() << QString("Set the turning direction %1").arg(message);
    TurningDirection::Enum turningDirection = TurningDirection::fromString(message.toUpper());
//    if (turningDirection != TurningDirection::UNDEFINED) {
//        if ((m_currentController == ExperimentControllerType::CIRCULAR_SETUP_LEADER_CCW)
//                && (turningDirection == TurningDirection::CLOCK_WISE))
//        {
//            setController(ExperimentControllerType::CIRCULAR_SETUP_LEADER_CW);
//        };
//        if ((m_currentController == ExperimentControllerType::CIRCULAR_SETUP_LEADER_CW)
//                && (turningDirection == TurningDirection::COUNTER_CLOCK_WISE))
//        {
//            setController(ExperimentControllerType::CIRCULAR_SETUP_LEADER_CCW);
//        };
//
//        if ((m_currentController == ExperimentControllerType::CIRCULAR_SETUP_LEADER_CCW_MODEL)
//                && (turningDirection == TurningDirection::CLOCK_WISE))
//        {
//            setController(ExperimentControllerType::CIRCULAR_SETUP_LEADER_CW_MODEL);
//        };
//        if ((m_currentController == ExperimentControllerType::CIRCULAR_SETUP_LEADER_CW_MODEL)
//                && (turningDirection == TurningDirection::COUNTER_CLOCK_WISE))
//        {
//            setController(ExperimentControllerType::CIRCULAR_SETUP_LEADER_CCW_MODEL);
//        };
//    } else {
//        qDebug() << "Can't set the turning direction for the message" << message;
//    }


    switch (turningDirection) {
    case TurningDirection::CLOCK_WISE:
        setController(ExperimentControllerType::CIRCULAR_SETUP_LEADER_CW_MODEL);
        break;
    case TurningDirection::COUNTER_CLOCK_WISE:
        setController(ExperimentControllerType::CIRCULAR_SETUP_LEADER_CCW_MODEL);
        break;
    default:
        setController(ExperimentControllerType::CIRCULAR_SETUP_FOLLOWER_MODEL);
        break;
    }
}

void ExperimentManager::setInitiationLurePreferedAreaId(Qstring preferedAreaId) {
    m_controllers[ExperimentControllerType::INITIATION_LURE]->setPreferedAreaId(preferedAreaId);
}


