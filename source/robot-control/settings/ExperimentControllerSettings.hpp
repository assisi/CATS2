#ifndef CATS2_EXPERIMENT_CONTROLLER_SETTINGS_HPP
#define CATS2_EXPERIMENT_CONTROLLER_SETTINGS_HPP

#include "experiment-controllers/ExperimentControllerType.hpp"

/*!
 * The parent class for settings for various controllers.
 */
class ExperimentControllerSettings
{
public:
    //! Constructor.
    explicit ExperimentControllerSettings(ExperimentControllerType::Enum type);
    //! Destructor.
    virtual ~ExperimentControllerSettings();

public:
    //! Initializes the parameters from the configuration file.
    //! Returns true if the parameters are considered valid.
    virtual bool init(QString configurationFileName) = 0;
    //! The tracking method.
    ExperimentControllerType::Enum type() const { return m_controllerType; }

protected:
    //! The controller for which these settings are applied.
    ExperimentControllerType::Enum m_controllerType;
    //! The section name in the configuration file.
    QString m_settingPathPrefix;
};

#endif // CATS2_EXPERIMENT_CONTROLLER_SETTINGS_HPP
