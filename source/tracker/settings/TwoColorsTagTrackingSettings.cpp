#include "TwoColorsTagTrackingSettings.hpp"

#include <QtCore/QDir>

/*!
 * Constructor.
 */
TwoColorsTagTrackingSettings::TwoColorsTagTrackingSettings(SetupType::Enum setupType) :
    TrackingRoutineSettings(setupType)
{
    m_trackingRoutineType = TrackingRoutineType::TWO_COLORS_TAG_TRACKING;
}

/*!
 * Initialization of the parameters for this specific method of tracking.
 * A part of the parameters is read from the robots' related section of the
 * settings file.
 */
bool TwoColorsTagTrackingSettings::init(QString configurationFileName)
{
    // read the settings
    ReadSettingsHelper settings(configurationFileName);

    readTagGroup(settings,
                 TwoColorsTagTrackingSettingsData::TagType::FRONT,
                 "frontTag");
    readTagGroup(settings,
                 TwoColorsTagTrackingSettingsData::TagType::BACK,
                 "backTag");

    // read the center position
    double centerProportionalPosition;
    settings.readVariable(QString("%1/tracking/centerProportionalPosition")
                          .arg(m_settingPathPrefix),
                          centerProportionalPosition,
                          m_data.centerProportionalPosition());
    m_data.setCenterProportionalPosition(centerProportionalPosition);

    return true;
}

void TwoColorsTagTrackingSettings::readTagGroup(ReadSettingsHelper& settings,
                                                TwoColorsTagTrackingSettingsData::TagType type,
                                                QString section)
{
    TwoColorsTagTrackingSettingsData::TagGroupDescription description;
    settings.readVariable(QString("%1/tracking/%2/tagsNumber")
                          .arg(m_settingPathPrefix)
                          .arg(section), description.numberOfTags, description.numberOfTags);
    // read the robot led's color
    int red;
    settings.readVariable(QString("%1/tracking/%2/tagColor/r")
                          .arg(m_settingPathPrefix)
                          .arg(section), red);
    int green;
    settings.readVariable(QString("%1/tracking/%2/tagColor/g")
                          .arg(m_settingPathPrefix)
                          .arg(section), green);
    int blue;
    settings.readVariable(QString("%1/tracking/%2/tagColor/b")
                          .arg(m_settingPathPrefix)
                          .arg(section), blue);
    description.tagColor = QColor(red, green, blue);
    // read the color threshold
    settings.readVariable(QString("%1/tracking/%2/threshold")
                          .arg(m_settingPathPrefix)
                          .arg(section),
                          description.colorThreshold, description.colorThreshold);
    m_data.addTagGroupDescription(type, description);
}
