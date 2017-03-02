#ifndef CATS2_ROBOT_CONTROL_POINTER_TYPES_HPP
#define CATS2_ROBOT_CONTROL_POINTER_TYPES_HPP

#include <QtCore/QSharedPointer>

/*!
 * The alias for the shared pointer to the fishbot.
 */
class FishBot;
using FishBotPtr = QSharedPointer<FishBot>;

/*!
 * The alias for the shared pointer to the dbus aseba interface.
 */
class DBusInterface;
using DBusInterfacePtr = QSharedPointer<DBusInterface>;

/*!
 * The alias for the shared pointer to the control target.
 */
class ControlTarget;
using ControlTargetPtr = QSharedPointer<ControlTarget>;

/*!
 * The alias for the shared pointer to the control mode.
 */
class ControlMode;
using ControlModePtr = QSharedPointer<ControlMode>;

/*!
 * The alias for the shared pointer to the control loop class.
 */
class ControlLoop;
using ControlLoopPtr = QSharedPointer<ControlLoop>;

/*!
 * The alias for the shared pointer to the robot handler class.
 */
class RobotsHandler;
using RobotsHandlerPtr = QSharedPointer<RobotsHandler>;

/*!
 * The alias for the shared pointer to the experiment controller class.
 */
class ExperimentController;
using ExperimentControllerPtr = QSharedPointer<ExperimentController>;

/*!
 * The alias for the shared pointer to potential field obstacle avoidance routine.
 */
class PotentialField;
using PotentialFieldPtr = QSharedPointer<PotentialField>;

/*!
 * The alias for the shared pointer to the control area class.
 */
class ControlArea;
using ControlAreaPtr = QSharedPointer<ControlArea>;

/*!
 * The alias for the shared pointer to the controller settings class.
 */
class ExperimentControllerSettings;
using ExperimentControllerSettingsPtr = QSharedPointer<ExperimentControllerSettings>;

#endif // CATS2_ROBOT_CONTROL_POINTER_TYPES_HPP
