#ifndef CATS2_TWO_COLORS_TAG_TRACKING_HPP
#define CATS2_TWO_COLORS_TAG_TRACKING_HPP

#include "TrackingRoutine.hpp"
#include "settings/TwoColorsTagTrackingSettings.hpp"
#include "TrackerPointerTypes.hpp"


/*!
 * Tracks two colors tags where tags of one colour are placed on the agent front
 * and the other colour - the back. The orientation is defined as a line between
 * the centers of mass of two groups of tags and the center as a point on this
 * line shifted by a predefined coefficient. It supports only one agent at the
 * moment.
 */
class TwoColorsTagTracking : public TrackingRoutine
{
public:
    //! Constructor. Gets the settings, the input queue to process and a queue
    //! to place debug images on request.
    explicit TwoColorsTagTracking(TrackingRoutineSettingsPtr settings,
                                  TimestampedFrameQueuePtr inputQueue,
                                  TimestampedFrameQueuePtr debugQueue);
    //! Destructor.
    virtual ~TwoColorsTagTracking();

    //! Reports on what type of agent can be tracked by this routine.
    virtual QList<AgentType> capabilities() const override;

    //! Getter for the settings.
    const TwoColorsTagTrackingSettingsData& settings() const { return m_settings; }
    //! Updates the settings.
    void setSettings(const TwoColorsTagTrackingSettingsData& settings);

protected:
    //! The tracking routine excecuted. Gets the original frame, detects
    //! tags, eventually associates them with the trajectories and
    //! enqueue debug images on request. Overriden from TrackingRoutine.
    virtual void doTracking(const TimestampedFrame& frame) override;

private:
    //! The tracking settings.
    TwoColorsTagTrackingSettingsData m_settings;
    //! Searches for the given tag group, returns it's centre of mass and the
    //! success status.
    bool detectTags(TwoColorsTagTrackingSettingsData::TagType tagType,
                           cv::Point2f& tagGroupCenter);

private:
    //! The intermediate data.
    //! The binary mask image.
    cv::Mat m_maskImage;
    //! The image after blurring.
    cv::Mat m_blurredImage;
    //! The image in HSV format.
    cv::Mat m_hsvImage;
    //! The binary image after threshold was applied.
    cv::Mat m_binaryImage;
    //! The image to put the debug information.
    cv::Mat m_debugImage;
};

#endif // CATS2_TWO_COLORS_TAG_TRACKING_HPP
