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
                         ExperimentControllerPtr(new ExperimentController(m_robot)));

    // and then the rest of controllers available in the settings
    for (auto controllerType : RobotControlSettings::get().availableControllers()) {
        m_controllers.insert(controllerType,
            ExperimentControllerFactory::createController(controllerType, m_robot));
    }

    // make connections
    connect(m_controllers[m_currentController].data(), &ExperimentController::notifyPolygons,
            this, &ExperimentManager::notifyPolygons);
}

/*!
 * Sets the requested controller.
 */
void ExperimentManager::setController(ExperimentControllerType::Enum type)
{
    if (!m_controllers.contains(type)) {
        qDebug() << Q_FUNC_INFO << "Requested controller is not available";
        return;
    }

    if (type != m_currentController) {
        qDebug() << QString("Changing the controller from %1 to %2 for %3")
                    .arg(ExperimentControllerType::toString(m_currentController))
                    .arg(ExperimentControllerType::toString(type))
                    .arg(m_robot->name());

        // disable connections
        disconnect(m_controllers[m_currentController].data(), &ExperimentController::notifyPolygons,
                this, &ExperimentManager::notifyPolygons);
        // first stop the current controller
        m_controllers[m_currentController]->finish();

        // then start the new one
        m_currentController = type;
        // add connections
        connect(m_controllers[m_currentController].data(), &ExperimentController::notifyPolygons,
                this, &ExperimentManager::notifyPolygons);
        // prepare the controller
        m_controllers[m_currentController]->start();
        // inform about changes
        emit notifyControllerChanged(type);
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
