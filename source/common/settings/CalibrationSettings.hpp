#ifndef CATS2_CALIBRATION_SETTINGS_HPP
#define CATS2_CALIBRATION_SETTINGS_HPP

#include "CommonPointerTypes.hpp"
#include "SetupType.hpp"

/*!
 * Class-signleton that is used to store parameters of the calibration.
 * Their values are loaded from the configuration file.
 * NOTE : All the settings are made as singltons to simplify the access to them;
 * the drawback is that initialization of many objects becomes obscure because of this.
 * NOTE : All the settings must be initialized on the program startup.
*/
class CalibrationSettings
{
public:
    //! The singleton getter. Provides an instance of the settings corresponding
    //! to the type of setup : main camera or the below camera.
    static CalibrationSettings& get();

    //! Initializes the parameters from the configuration file.
    bool init(QString configurationFileName, SetupType::Enum setupType);

    // delete copy and move constructors and assign operators
    //! Copy constructor.
    CalibrationSettings(CalibrationSettings const&) = delete;
    //! Move constructor.
    CalibrationSettings(CalibrationSettings&&) = delete;
    //! Copy assignment.
    CalibrationSettings& operator=(CalibrationSettings const&) = delete;
    //! Move assignment.
    CalibrationSettings& operator=(CalibrationSettings &&) = delete;

public:
    //! Returns the path to the camera calibration file.
    QString calibrationFilePath(SetupType::Enum setupType) const { return m_calibrationFilePaths[setupType]; }

private:
    //! Constructor. Defining it here prevents construction.
    CalibrationSettings() {}
    //! Destructor. Defining it here prevents unwanted destruction.
    ~CalibrationSettings() {}

private:
    //! Stores paths to calibration files for every available setup.
    QMap<SetupType::Enum, QString> m_calibrationFilePaths;
};


#endif // CATS2_CALIBRATION_SETTINGS_HPP
