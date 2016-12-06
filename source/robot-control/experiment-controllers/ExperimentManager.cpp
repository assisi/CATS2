#include "ExperimentManager.hpp"

#include "FishBot.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
ExperimentManager::ExperimentManager(FishBot* robot, QObject *parent) :
    QObject(parent),
    m_currentController(ExperimentControllerType::NONE),
    m_robot(robot)
{

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

        // first stop the current control mode
        m_controllers[m_currentController]->finish();

        // then start the new one
        m_currentController = type;
        m_controllers[m_currentController]->start();

        emit notifyControllerChanged(type);
    }
}

/*!
 * The experiment manager step. Returns the control target values.
 */
ExperimentController::ControlData ExperimentManager::step()
{
    return m_controllers[m_currentController]->step();
}
