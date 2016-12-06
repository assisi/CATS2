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
namespace Aseba {
    class DBusInterface;
    using DBusInterfacePtr = QSharedPointer<DBusInterface>;
}

/*!
 * The alias for the shared pointer to control target.
 */
class ControlTarget;
using ControlTargetPtr = QSharedPointer<ControlTarget>;

/*!
 * The alias for the shared pointer to control mode.
 */
class ControlMode;
using ControlModePtr = QSharedPointer<ControlMode>;

/*!
 * The alias for the shared pointer to control loop class.
 */
class ControlLoop;
using ControlLoopPtr = QSharedPointer<ControlLoop>;

/*!
 * The alias for the shared pointer to robot handler class.
 */
class RobotsHandler;
using RobotsHandlerPtr = QSharedPointer<RobotsHandler>;

/*!
 * The alias for the shared pointer to experiment controller class.
 */
class ExperimentController;
using ExperimentControllerPtr = QSharedPointer<ExperimentController>;

#endif // CATS2_ROBOT_CONTROL_POINTER_TYPES_HPP
