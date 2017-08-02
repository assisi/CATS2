#ifndef CATS2_SETTINGS_HPP
#define CATS2_SETTINGS_HPP

#include <SettingsInterface.hpp>

#include <SetupType.hpp>

#include <QtCore/QDebug>

class GrabberSettings;

/*!
 * Class-signleton that is used to bring together all the signleton settings classes,
 * thus hopefully simplifying access to them. It also stores the fundamental flags that
 * define the functioning of the application.
 *
 * TODO : when using singletons things easily go out of control. Consider refactoring the settings
 * code across the whole project to get rid of singletons.
 */
class Settings
{
public:
    //! The singleton getter.
    static Settings& get();

    //! Initializes the parameters from the configuration file.
    bool init(int argc, char *argv[],
              bool needConfigFile = true, bool needCalibration = true,
              bool hasMainCamera = true, bool hasBelowCamera = false,
              bool needPublisherAddress = false, bool needRobots = false);

    // delete copy and move constructors and assign operators
    //! Copy constructor.
    Settings(Settings const&) = delete;
    //! Move constructor.
    Settings(Settings&&) = delete;
    //! Copy assignment.
    Settings& operator=(Settings const&) = delete;
    //! Move assignment.
    Settings& operator=(Settings &&) = delete;

    //! Returns the reference to the grabber settings.
    GrabberSettings& grabberSettings();

public:
    //! Checks if the setup is set as available.
    bool isAvailable(SetupType::Enum setupType);

private:
    //! Constructor. Defining it here prevents construction.
    Settings() {}
    //! Destructor. Defining it here prevents unwanted destruction.
    ~Settings() {}

private:
    //! The interface to provide settings values to external services.
    SettingsInterfacePtr m_settingsInterface;
};

#endif // CATS2_SETTINGS_HPP
