#ifndef CATS2_COLOR_DETECTOR_SETTINGS_HPP
#define CATS2_COLOR_DETECTOR_SETTINGS_HPP

#include "TrackingRoutineSettings.hpp"

#include <QtGui/QColor>

/*!
 * The actual data stored in the settings. It's separated in a class to be
 * easily trasferable to the corresponding tracking routine.
 */
class ColorDetectorSettingsData
{
public:
    //! Constructor.
    explicit ColorDetectorSettingsData() :
        m_numberOfAgents(0),
        m_color(0, 0, 0),
        m_colorThreshold(20),
        m_maskFilePath()
    {}

public:
    //! Return the number of agents to be tracked.
    int numberOfAgents() const { return m_numberOfAgents; }
    //! Set the number of agents.
    void setNumberOfAgents(int numberOfAgents) { m_numberOfAgents = numberOfAgents; }

    //! Return the color to be tracked.
    QColor color() const { return m_color; }
    //! Set the color to be tracked.
    void setColor(QColor color) { m_color = color; }

    //! Return the color detection threshold.
    int threshold() const { return m_colorThreshold; }
    //! Set the color detection threshold.
    void setThreshold(int threshold) { m_colorThreshold = threshold; }

    std::string maskFilePath() const { return m_maskFilePath; }
    void setMaskFilePath(std::string maskFilePath) { m_maskFilePath = maskFilePath; }

protected:
    //! Number of agents to track.
    int m_numberOfAgents;
    //! The color to look for.
    QColor m_color;
    //! The color detection threshold.
    int m_colorThreshold;
    //! The arena mask file, an optional parameter.
    std::string m_maskFilePath;
};

/*!
 * The settings for the blob detector routine.
 */
class ColorDetectorSettings : public TrackingRoutineSettings
{
public:
    //! Constructor.
    explicit ColorDetectorSettings(SetupType::Enum setupType);

public:
    //! Initialization of the parameters for this specific method of tracking.
    //! Returns true if the parameters are considered valid.
    virtual bool init(QString configurationFileName) override;

    //! Provides a copy of the settings data.
    ColorDetectorSettingsData data() { return m_data; }

private:
    //! The settings data.
    ColorDetectorSettingsData m_data;
};

#endif // CATS2_COLOR_DETECTOR_SETTINGS_HPP
