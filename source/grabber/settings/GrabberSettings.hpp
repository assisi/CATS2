#ifndef CATS2_GRABBER_SETTINGS_HPP
#define CATS2_GRABBER_SETTINGS_HPP

#include "CommonPointerTypes.hpp"
#include "SetupType.hpp"

#include <QtCore/QSize>

/*!
 * Class-signleton that is used to store parameters of the grabber.
 * Their values are loaded from the configuration file.
 * NOTE : All the settings are made as singltons to simplify the access to them;
 * the drawback is that initialization of many objects becomes obscure because of this.
 * NOTE : All the settings must be initialized on the program startup.
*/
class GrabberSettings
{
public:
    //! The singleton getter. Provides an instance of the settings corresponding
    //! to the type of setup : main camera or the below camera.
    static GrabberSettings& get();

    //! Initializes the parameters from the configuration file.
    bool init(QString configurationFileName, SetupType::Enum setupType, bool needTargetFrameSize = false);

    // delete copy and move constructors and assign operators
    //! Copy constructor.
    GrabberSettings(GrabberSettings const&) = delete;
    //! Move constructor.
    GrabberSettings(GrabberSettings&&) = delete;
    //! Copy assignment.
    GrabberSettings& operator=(GrabberSettings const&) = delete;
    //! Move assignment.
    GrabberSettings& operator=(GrabberSettings &&) = delete;

public:
    //! Returns the size of the video frames.
    QSize frameSize(SetupType::Enum setupType) const { return m_targetFrameSizes[setupType]; }

private:
    //! Constructor. Defining it here prevents construction.
    GrabberSettings() {}
    //! Destructor. Defining it here prevents unwanted destruction.
    ~GrabberSettings() {}

private:
    //! Stores the target video frame sizes for every available setup. It's used to
    //! interpolate the calibration values if they were measured for another frame size.
    QMap<SetupType::Enum, QSize> m_targetFrameSizes;
};


#endif // CATS2_GRABBER_SETTINGS_HPP
