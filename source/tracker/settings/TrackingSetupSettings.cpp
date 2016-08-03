#include "TrackingSetupSettings.hpp"

#include "TrackingSettings.hpp"

#include <settings/CommandLineParameters.hpp>
#include <settings/CalibrationSettings.hpp>

/*!
 * Initializes the parameters for the tracking setup of given type.
 */
bool TrackingSetupSettings::init(SetupType::Enum setupType)
{
    QString configurationFilePath = CommandLineParameters::get().configurationFilePath();
    // first check that the calibration settings are initialized
    if (CalibrationSettings::get().init(configurationFilePath, setupType)) {
        // then check that the input stream is set
        if (CommandLineParameters::get().cameraDescriptor(setupType).isValid()) {
            // finally check that the tracking routine is set for given setup type
            if (TrackingSettings::get().init(configurationFilePath, setupType)) {
                return true;
            } else {
                qDebug() << Q_FUNC_INFO << "Couldn't setup the tracking routine, finished";
                return false;
            }
        } else {
            qDebug() << Q_FUNC_INFO << "Main camera descriptor is ill-defined";
            return false;
        }
    } else {
        qDebug() << Q_FUNC_INFO << "Couldn't setup calibration settings, finished";
        return false;
    }
}



