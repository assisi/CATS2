#ifndef CATS2_TRACKING_ROUTINE_SETTINGS_HPP
#define CATS2_TRACKING_ROUTINE_SETTINGS_HPP

#include "routines/TrackingRoutine.hpp"
#include "routines/TrackingRoutineType.hpp"

#include <SetupType.hpp>

#include <QtCore/QString>
#include <QtCore/QSharedPointer>

/*!
 * The parent class for settings for various tracing routines.
 */
class TrackingRoutineSettings
{
public:
    //! Constructor.
    explicit TrackingRoutineSettings(SetupType setupType);
    //! Destructor.
    virtual ~TrackingRoutineSettings();

public:
    //! Initializes the parameters from the configuration file. Returns true if the parameters are
    //! considered valid.
    virtual bool init(QString configurationFileName) = 0;
    //! The tracking method.
    TrackingRoutineType type() const { return _trackingRoutineType; }

protected:
    //! The tracking method for which these settings are applied.
    TrackingRoutineType _trackingRoutineType;
    //! The section name in the configuration file.
    QString _settingPathPrefix;
};

/*!
 * The alias for the pointer to the tracking routine settings class.
 */
using TrackingRoutineSettingsPtr = QSharedPointer<TrackingRoutineSettings>;

#endif // CATS2_TRACKING_ROUTINE_SETTINGS_HPP
