#include "ExperimentControllerSettings.hpp"

#include <QtCore/QDir>
#include <QtCore/QDebug>

#include <settings/ReadSettingsHelper.hpp>

/*!
 * Constructor.
 */
ExperimentControllerSettings::ExperimentControllerSettings(ExperimentControllerType::Enum type
                                                           = ExperimentControllerType::NONE) :
    m_controllerType(type)
{
    m_settingPathPrefix = QString("robots/controllers/%1")
            .arg(ExperimentControllerType::toSettingsString(m_controllerType));
}

/*!
 * Destructor.
 */
ExperimentControllerSettings::~ExperimentControllerSettings()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
}

