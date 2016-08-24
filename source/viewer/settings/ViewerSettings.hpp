#ifndef CATS2_VIEWER_SETTINGS_HPP
#define CATS2_VIEWER_SETTINGS_HPP

#include "CommonPointerTypes.hpp"
#include "SetupType.hpp"

#include <QtCore/QSize>

/*!
 * Class-signleton that is used to store parameters of the viewer.
 * Their values are loaded from the configuration file.
 * NOTE : All the settings are made as singltons to simplify the access to them;
 * the drawback is that initialization of many objects becomes obscure because of this.
 * NOTE : All the settings must be initialized on the program startup.
*/
class ViewerSettings
{
public:
    //! The singleton getter. Provides an instance of the settings corresponding
    //! to the type of setup : main camera or the below camera.
    static ViewerSettings& get();

    //! Initializes the parameters from the configuration file.
    bool init(QString configurationFileName, SetupType::Enum setupType, bool needTargetFrameSize = true);

    // delete copy and move constructors and assign operators
    //! Copy constructor.
    ViewerSettings(ViewerSettings const&) = delete;
    //! Move constructor.
    ViewerSettings(ViewerSettings&&) = delete;
    //! Copy assignment.
    ViewerSettings& operator=(ViewerSettings const&) = delete;
    //! Move assignment.
    ViewerSettings& operator=(ViewerSettings &&) = delete;

public:
    //! Returns the size of the video frames.
    QSize frameSize(SetupType::Enum setupType) const { return m_targetFrameSizes[setupType]; }

private:
    //! Constructor. Defining it here prevents construction.
    ViewerSettings() {}
    //! Destructor. Defining it here prevents unwanted destruction.
    ~ViewerSettings() {}

private:
    //! Stores the target video frame sizes for every available setup. It's used to
    //! interpolate the calibration values if they were measured for another frame size.
    QMap<SetupType::Enum, QSize> m_targetFrameSizes;
};


#endif // CATS2_VIEWER_SETTINGS_HPP
