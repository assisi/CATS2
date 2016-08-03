#ifndef CATS2_TRACKING_SETUP_SETTINGS_HPP
#define CATS2_TRACKING_SETUP_SETTINGS_HPP

#include <SetupType.hpp>

/*!
 * Service class that simplifies the initialization of the the settings for the tracking setup.
 */
class TrackingSetupSettings
{
public:
    //! Initializes the parameters for the tracking setup.
    static bool init(SetupType::Enum setupType);
};

#endif // CATS2_TRACKING_SETUP_SETTINGS_HPP
