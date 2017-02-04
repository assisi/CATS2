#include "ControlMode.hpp"
#include "FishBot.hpp"
#include "ControlTarget.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
ControlMode::ControlMode(FishBot* robot, ControlModeType::Enum type):
    QObject(nullptr),
    m_robot(robot),
    m_type(type)
{

}

/*!
 * Destructor.
 */
ControlMode::~ControlMode()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
}

/*!
 * Informs on what kind of control targets this control mode generates.
 */
QList<ControlTargetType> ControlMode::supportedTargets()
{
    return QList<ControlTargetType>();
}
