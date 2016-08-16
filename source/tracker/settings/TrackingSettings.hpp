#ifndef CATS2_TRACKING_SETTINGS_HPP
#define CATS2_TRACKING_SETTINGS_HPP

#include "TrackingRoutineSettings.hpp"

#include "TrackingSetup.hpp"
#include <SetupType.hpp>

/*!
 * Class-signleton that is used to store parameters of the tracking.
 * Their values are loaded from the configuration file.
 * NOTE : All the settings are made as singltons to simplify the access to them;
 * the drawback is that initialization of many objects becomes obscure because of this.
 * NOTE : All the settings must be initialized on the program startup.
*/
class TrackingSettings
{
public:
    //! The singleton getter. Provides an instance of the settings corresponding
    //! to the type of setup : main camera or the below camera.
    static TrackingSettings& get();

    //! Initializes the parameters from the configuration file.
    bool init(QString configurationFileName, SetupType::Enum setupType);

    // delete copy and move constructors and assign operators
    //! Copy constructor.
    TrackingSettings(TrackingSettings const&) = delete;
    //! Move constructor.
    TrackingSettings(TrackingSettings&&) = delete;
    //! Copy assignment.
    TrackingSettings& operator=(TrackingSettings const&) = delete;
    //! Move assignment.
    TrackingSettings& operator=(TrackingSettings &&) = delete;

public:
    /*!
     * Returns the settings for the tracking routine used in this setup.
     */
    TrackingRoutineSettingsPtr trackingRoutineSettings(SetupType::Enum type) const
    {
        return m_trackingRoutineSettings.value(type);
    }

private:
    //! Constructor. Defining it here prevents construction.
    TrackingSettings() {}
    //! Destructor. Defining it here prevents unwanted destruction.
    ~TrackingSettings() {}

private:
    //! Reads from the configuration file the tracking routine type corresponding to the setup
    //! type of this instance.
    TrackingRoutineType::Enum readTrackingRoutineType(QString configurationFileName, SetupType::Enum setupType);

private:
    //! The settings for the tracking routine used in various setups.
    QMap<SetupType::Enum, TrackingRoutineSettingsPtr> m_trackingRoutineSettings;
};

#endif // CATS2_TRACKING_SETTINGS_HPP
