#ifndef CATS2_TWO_COLORS_TAG_TRACKING_SETTINGS_HPP
#define CATS2_TWO_COLORS_TAG_TRACKING_SETTINGS_HPP

#include "TrackingRoutineSettings.hpp"

#include <settings/ReadSettingsHelper.hpp>

#include <QtCore/QDebug>
#include <QtGui/QColor>

/*!
 * The actual data stored in the settings. It's separated in a class to be
 * easily trasferable to the corresponding tracking routine.
 */
class TwoColorsTagTrackingSettingsData
{
public:
    //! Constructor.
    explicit TwoColorsTagTrackingSettingsData() :
        m_centerProportionalPosition(0.5)
    { }

public:
    enum class TagType
    {
        FRONT,
        BACK
    };

    //! The structure that stores the tracking parameters for one tag group.
    struct TagGroupDescription
    {
        //! Initialization.
        TagGroupDescription() :
            numberOfTags(1),
            tagColor(0,0,0),
            colorThreshold(100)
        {}
        //! The number of tags.
        ushort numberOfTags;
        //! The tag's color.
        QColor tagColor;
        //! The color detection threshold.
        ushort colorThreshold;
    };

    //! Returns the tag group description.
    TagGroupDescription tagGroupDescription(TagType tagType) const
    {
        if (m_tagGroupDescription.contains(tagType))
            return m_tagGroupDescription[tagType];
        else {
            qDebug() << "Requesting the unsopported tag type";
            return TagGroupDescription();
        }
    }

    //! Adds a tags group description of the given type.
    void updateTagGroupDescription(TagType tagType,
                                   TagGroupDescription description)
    {
        m_tagGroupDescription[tagType] = description;
    }

    //! Set the center proportional position.
    void setCenterProportionalPosition(double centerProportionalPosition)
    {
        m_centerProportionalPosition = centerProportionalPosition;
    }

    //! Get the center proportional position.
    double centerProportionalPosition() const { return m_centerProportionalPosition; }

protected:
    //! The parameters of all tags to track.
    QMap<TagType, TagGroupDescription> m_tagGroupDescription;
    //! The proportional position of the robot's center with respect to the
    //! centers of the tag group.
    double m_centerProportionalPosition;
};

/*!
 * The settings for the two color tags tracking routine.
 */
class TwoColorsTagTrackingSettings : public TrackingRoutineSettings
{
public:
    //! Constructor.
    explicit TwoColorsTagTrackingSettings(SetupType::Enum setupType);

public:
    //! Initialization of the parameters for this specific method of tracking.
    //! Returns true if the parameters are considered valid.
    virtual bool init(QString configurationFileName) override;

    //! Provides a copy of the settings data.
    TwoColorsTagTrackingSettingsData data() { return m_data; }

private:
    //! Reads the data for one tag group.
    void readTagGroup(ReadSettingsHelper& settings,
                      TwoColorsTagTrackingSettingsData::TagType type,
                      QString section);
    //! The settings data.
    TwoColorsTagTrackingSettingsData m_data;
};


#endif // CATS2_TWO_COLORS_TAG_TRACKING_SETTINGS_HPP
