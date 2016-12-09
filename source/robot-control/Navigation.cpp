#include "Navigation.hpp"

#include "dbusinterface.h"
#include "control-modes/ControlTarget.hpp"
#include "FishBot.hpp"

/*!
 * Constructor.
 */
Navigation::Navigation(FishBot* robot):
    QObject(nullptr),
    m_robot(robot),
    m_motionPattern(MotionPatternType::PID),
    m_fishMotionPatternSettings(RobotControlSettings::get().fishMotionPatternSettings()),
    m_fishMotionFrequencyDivider(RobotControlSettings::get().fishMotionPatternFrequencyDivider()),
    m_fishMotionStepCounter(0),
    m_pidControllerSettings(RobotControlSettings::get().pidControllerSettings()),
    m_dt(1. / RobotControlSettings::get().controlFrequencyHz())
{

}

/*!
 * Destructor.
 */
Navigation::~Navigation()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
    // stop the robot before quiting
    sendMotorSpeed(0, 0);
}

/*!
 * The navigation step. Gets new control target and converts it to the motor
 * commands.
 */
void Navigation::step(ControlTargetPtr target)
{
    switch (target->type()) {
    case ControlTargetType::SPEED:
        setTargetSpeed(static_cast<TargetSpeed*>(target.data()));
        break;
    case ControlTargetType::POSITION:
        setTargetPosition(static_cast<TargetPosition*>(target.data()));
        break;
    default:
        break;
    }
}

/*!
 * Manages the target speed control.
 */
void Navigation::setTargetSpeed(TargetSpeed* targetSpeed)
{
    sendMotorSpeed(targetSpeed->leftSpeed(), targetSpeed->rightSpeed());
}

/*!
 * Manages the target position control.
 */
void Navigation::setTargetPosition(TargetPosition* targetPostion)
{
    // FIXME : temporary code
    emit notifyTargetPositionChanged(targetPostion->position());
    switch (m_motionPattern) {
    case MotionPatternType::FISH_MOTION:
        fishMotionToTargetPosition(targetPostion);
        break;
    case MotionPatternType::PID:
    default:
        pidControlToTargetPosition(targetPostion);
        break;
    }
}

/*!
 * Sends the motor speed to the robot.
 */
void Navigation::sendMotorSpeed(int leftSpeed, int rightSpeed)
{
    // TODO : to add some safety checks on the speed and eventually convertions

    // event to send
    QString eventName;
    // data to send
    Values data;

    eventName = "MotorControl" + m_robot->name();
    data.append(leftSpeed);
    data.append(rightSpeed);
    m_robot->robotInterface()->sendEventName(eventName, data);
}

/*!
 * Sends the motor speed to the robot. It's converted from the angular speed.
 */
void Navigation::sendMotorSpeed(double angularSpeed)
{
    int linearSpeed = RobotControlSettings::get().defaultLinearSpeedCmSec();
    int leftSpeed =  linearSpeed + (angularSpeed * FishBot::InterWheelsDistanceCm) / 2.0;
    int rightSpeed = linearSpeed - (angularSpeed * FishBot::InterWheelsDistanceCm) / 2.0;
    sendMotorSpeed(leftSpeed, rightSpeed);
}

/*!
 * Sends the fish behavior parameters to the robot.
 * angle : angle to turn to the target.
 * distance : a parameter that defines the acceleration
 * speed : the speed after the acceleration
 */
void Navigation::sendFishMotionParameters(int angle, int distance, int speed)
{
    // event to send
    QString eventName;
    // data to send
    Values data;

    // bound the angle
    if(angle >100)
        angle = 100;
    else if (angle <-100)
        angle = -100;

    // TODO : to check other parameters

    eventName = "FishBehavior" + m_robot->name();
    data.append(angle);
    data.append(distance);
    data.append(speed);
    m_robot->robotInterface()->sendEventName(eventName, data);
}

/*!
 * Computes the turn angle based on the robot's orientation, position
 * and the target position.
 */
double Navigation::computeAngleToTurn(TargetPosition* targetPostion)
{
    // FIXME : consider making calculations in the body frame
    //    double robotCenteredTargetPositionX = qCos(robotOrientation) * dx - qSin(robotOrientation) * dy;
    //    double robotCenteredTargetPositionY = qSin(robotOrientation) * dx + qCos(robotOrientation) * dy;
    //    double angleToTurn = qAtan2(robotCenteredTargetPositionY, robotCenteredTargetPositionX);

    double dx = targetPostion->position().x() - m_robot->state().position().x();
    double dy = targetPostion->position().y() - m_robot->state().position().y();
    double angleToTarget = qAtan2(dy, dx);
    double robotOrientation = m_robot->state().orientation().angleRad();
    double angleToTurn = robotOrientation - angleToTarget;
    // normalize to [-pi;pi]
    if (angleToTurn < - M_PI)
        angleToTurn += 2 * M_PI;
    if (angleToTurn > M_PI)
        angleToTurn -= 2 * M_PI;

    return angleToTurn;
}

/*!
 * Excecutes fish motion pattern while going to target.
 */
void Navigation::fishMotionToTargetPosition(TargetPosition* targetPostion)
{
    if (m_robot->state().position().isValid()
            && m_robot->state().orientation().isValid()
            && targetPostion->position().isValid()) {
        m_fishMotionStepCounter++;
        // if we waited enough steps
        if (m_fishMotionStepCounter >=  m_fishMotionFrequencyDivider) {
            m_fishMotionStepCounter = 0;
            double angleToTurn = computeAngleToTurn(targetPostion);
            // FIXME : to check why the angle to send is negative
            sendFishMotionParameters(- angleToTurn * 180 / M_PI,
                                     m_fishMotionPatternSettings.distanceCm(), // FIXME : store this parameters somewhere in this class
                                     m_fishMotionPatternSettings.speedCmSec());
        }
    }
}

/*!
 * Excecutes PID while going to target.
 */
void Navigation::pidControlToTargetPosition(TargetPosition* targetPostion)
{
    if (m_robot->state().position().isValid()
            && m_robot->state().orientation().isValid()
            && targetPostion->position().isValid()) {
        double angleToTurn = computeAngleToTurn(targetPostion);
        // proportional term
        double proportionalTerm = angleToTurn;
        // derivative term
        double derivativeTerm = 0;
        if (m_errorBuffer.size() > 0)
            derivativeTerm = (angleToTurn - m_errorBuffer.last()) * m_dt;
        // integral term
        double integralTerm = 0;
        m_errorBuffer.enqueue(angleToTurn);
        if (m_errorBuffer.size() > ErrorBufferDepth) { // the buffer if full, i.e. we can use it
            // forget the oldest element
            m_errorBuffer.dequeue();
            // and sum the rest of them
            for (double error : m_errorBuffer)
                integralTerm += error;
        }
        double angularVelocity = m_pidControllerSettings.kp() * proportionalTerm +
                                 m_pidControllerSettings.ki() * integralTerm +
                                 m_pidControllerSettings.kd() * derivativeTerm;
        sendMotorSpeed(angularVelocity);
    }
}

/*!
 * Sets the requested motion pattern.
 */
void Navigation::setMotionPattern(MotionPatternType::Enum type)
{
    if (type != m_motionPattern) {
        qDebug() << QString("Changing the motion pattern from from %1 to %2 for %3")
                    .arg(MotionPatternType::toString(m_motionPattern))
                    .arg(MotionPatternType::toString(type))
                    .arg(m_robot->name());

        m_motionPattern = type;
        // reset the steps counter
        m_fishMotionStepCounter = 0;

        emit notifyMotionPatternChanged(type);
    }
}

/*!
 * Sets the frequency divider for the fish motion pattern. At the moment this
 * is supported for the fish motion pattern only, but it can be used for
 * other motion patterns as well.
 */
void Navigation::setMotionPatternFrequencyDivider(MotionPatternType::Enum type,
                                                  int frequencyDivider)
{
    if (type == MotionPatternType::FISH_MOTION) {
        if (frequencyDivider != m_fishMotionFrequencyDivider) {
            m_fishMotionFrequencyDivider = frequencyDivider;
            m_fishMotionStepCounter = 0;
            emit notifyMotionPatternFrequencyDividerChanged(MotionPatternType::FISH_MOTION,
                                                            frequencyDivider);
        }
    }
}

/*!
 * Return the frequency divider for the motion pattern.
 */
int Navigation::motionPatternFrequencyDivider(MotionPatternType::Enum type)
{
    if (type == MotionPatternType::FISH_MOTION) {
        return m_fishMotionFrequencyDivider;
    } else {
        qDebug() << QString("The motion frequency divider is not supported for %1")
                    .arg(MotionPatternType::toString(type));
        return 1;
    }
}

