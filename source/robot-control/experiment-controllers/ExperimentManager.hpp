#ifndef CATS2_EXPERIMENT_MANAGER_HPP
#define CATS2_EXPERIMENT_MANAGER_HPP

#include "ExperimentController.hpp"
#include "ExperimentControllerType.hpp"
#include "RobotControlPointerTypes.hpp"

#include <QtCore/QObject>
#include <QtCore/QMap>

class FishBot;

/*!
 * Manages the different experiment controllers.
 */
// TODO : to move this file/class to the main folder
class ExperimentManager: public QObject
{
    Q_OBJECT
public:
    //! Constructor.
    explicit ExperimentManager(FishBot* robot);

    //! The experiment manager step. Returns the control target values.
    ExperimentController::ControlData step();

    //! Return a list of all supported controllers.
    QList<ExperimentControllerType::Enum> supportedControllers() const { return m_controllers.keys(); }
    //! Return the current controller.
    ExperimentControllerType::Enum currentController() const { return m_currentController; }
    //! Is the experiment manager active.
    bool isActive() const { return (m_currentController != ExperimentControllerType::NONE); }

    //! Sets the circular setup robot turning direction (CW/CCW).
    void setCircularSetupTurningDirection(QString message);
    //! Sets preferedAreaId
    void setInitiationLurePreferedAreaId(Qstring preferedAreaId);

public slots:
    //! Sets the requested controller.
    void setController(ExperimentControllerType::Enum type);
    //! Requests to sends the map areas' polygons.
    void requestPolygons() { m_controllers[m_currentController]->requestPolygons(); }

signals:
    //! Informs that the robot's controller was modified.
    void notifyControllerChanged(ExperimentControllerType::Enum type);
    //! Sends out the current controller status.
    void notifyControllerStatus(QString status);
    //! Sends the map areas' polygons.
    void notifyPolygons(QList<AnnotatedPolygons>);
    //! Sends the areas' occupation by fish information.
    void notifyFishNumberByAreas(QMap<QString, int>);

signals: // experiment specific
    //! Informs the interspecies-module on the fish and robots turning
    //! directions in the circular setup experiment.
    void notifyCircularSetupTurningDirections(QString fishTurningDirection,
                                              QString robotTurningDirection);
    //! Informs the interspecies-module on the fish and robots statistics
    //! in the circular setup experiment.
    void notifyCircularSetupStatistics(double fishClockWisePercent,
                                       double fishCounterClockWisePercent,
                                       QString fishTurningDirection,
                                       double robotClockWisePercent,
                                       double robotCounterClockWisePercent,
                                       QString robotTurningDirection);

private:
    //! The list of experiment controllers available to this manager.
    QMap<ExperimentControllerType::Enum, ExperimentControllerPtr> m_controllers;

    //! The current control mode.
    ExperimentControllerType::Enum m_currentController;

    //! A pointer to the robot that is controlled by this state machine.
    FishBot* m_robot;
};

#endif // CATS2_EXPERIMENT_MANAGER_HPP
