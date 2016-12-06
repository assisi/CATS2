#ifndef CATS2_EXPERIMENT_MANAGER_HPP
#define CATS2_EXPERIMENT_MANAGER_HPP

#include "ExperimentController.hpp"
#include "ExperimentControllerType.hpp"
#include "RobotControlPointerTypes.hpp"

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QMap>

class FishBot;

/*!
 * Manages the different experiment controllers.
 */
class ExperimentManager: public QObject
{
    Q_OBJECT
public:
    //! Constructor.
    explicit ExperimentManager(FishBot* robot, QObject *parent = 0);

    //! The experiment manager step. Returns the control target values.
    ExperimentController::ControlData step();

signals:
    //! Informs that the robot's controller was modified.
    void notifyControllerChanged(ExperimentControllerType::Enum type);

public slots:
    //! Sets the requested controller.
    void setController(ExperimentControllerType::Enum type);

private:
    //! The list of experiment controllers available to this manager.
    QMap<ExperimentControllerType::Enum, ExperimentControllerPtr> m_controllers;

    //! The current control mode.
    ExperimentControllerType::Enum m_currentController;

    //! A pointer to the robot that is controlled by this state machine.
    FishBot* m_robot;
};

#endif // CATS2_EXPERIMENT_MANAGER_HPP
