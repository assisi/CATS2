#include "Manual.hpp"

#include "settings/RobotControlSettings.hpp"

#include <QtCore/QDebug>

#if defined(unix) || defined(__unix__) || defined(__unix)
#include <linux/input.h>
#include <linux/joystick.h>
#include <fcntl.h>
#include <unistd.h>
#endif

/*!
 * Constructor.
 */
Manual::Manual(FishBot* robot) : ControlMode(robot, ControlModeType::MANUAL)
{
    // open joystick port
    if ((m_descriptor = open("/dev/input/js0", O_RDONLY)) < 0) {
        qDebug() << "Could not open the joystick";
    }
}

/*!
 * Destructor.
 */
Manual::~Manual()
{
    qDebug() << "Destroying the object";
}

/*!
 * The step of the control mode, reads the joystic commands and sends further.
 */
ControlTargetPtr Manual::step()
{
#if defined(unix) || defined(__unix__) || defined(__unix)
    if (m_descriptor < 0) {
        return ControlTargetPtr(new TargetSpeed(0.0, 0.0));
    }

    double leftSpeed = 0., rightSpeed = 0.;
    // read joystick
    struct JS_DATA_TYPE joystickData;
    if (read(m_descriptor, &joystickData, JS_RETURN) != JS_RETURN) {
        qDebug() << "error reading joystick";
    } else {
        // TODO : clean this up
        joystickData.x = joystickData.x - 128;
        joystickData.x /= 2;
        joystickData.y = 128 - joystickData.y;

        double theta = atan2(joystickData.y, joystickData.x) - M_PI / JoystickThetaCorrection;
        double r = sqrt(joystickData.x * joystickData.x + joystickData.y * joystickData.y)/sqrt(2.0*127.*127); // FIXME : 127.?
        r *= RobotControlSettings::get().defaultLinearSpeedCmSec();

        leftSpeed = r * (cos(theta) - 0.5 * sin(theta));
        rightSpeed = r * (cos(theta) + 0.5 * sin(theta));
    }
    return ControlTargetPtr(new TargetSpeed(leftSpeed, rightSpeed));
#else
    return ControlTargetPtr(new TargetSpeed(0.0, 0.0));
#endif
}

/*!
 * Informs on what kind of control targets this control mode generates.
 */
QList<ControlTargetType> Manual::supportedTargets()
{
    return QList<ControlTargetType>({ControlTargetType::SPEED});
}

