#ifndef CATS2_COLOR_DETECTOR_HPP
#define CATS2_COLOR_DETECTOR_HPP

#include "TrackingRoutine.hpp"
#include "settings/ColorDetectorSettings.hpp"
#include "TrackerPointerTypes.hpp"

/*!
 * Tracks the colored spots.
 */
class ColorDetector : public TrackingRoutine
{
    Q_OBJECT

public:
    //! Constructor. Gets the settings, the input queue to process and a queue to place debug images on request.
    explicit ColorDetector(TrackingRoutineSettingsPtr settings, TimestampedFrameQueuePtr inputQueue, TimestampedFrameQueuePtr debugQueue);
    //! Destructor.
    virtual ~ColorDetector();

    //! Reports on what type of agent can be tracked by this routine.
    virtual QList<AgentType> capabilities() const override;

    //! Getter for the settings.
    const ColorDetectorSettingsData& settings() const { return m_settings; }
    //! Updates the settings.
    void setSettings(const ColorDetectorSettingsData& settings);

protected:
    //! The tracking routine excecuted. Gets the original frame, detects
    //! agents, eventually associates them with the trajectories and
    //! enqueue debug images on request. Overriden from TrackingRoutine.
    virtual void doTracking(const TimestampedFrame& frame) override;

private:
    //! The tracking settings.
    ColorDetectorSettingsData m_settings;
    //! Searches for the given robot's leds on the image.
    void detectLeds(size_t robotIndex);

private:
    //! The intermediate data.
    //! The binary mask image.
    cv::Mat m_maskImage;
    //! The image after blurring.
    cv::Mat m_blurredImage;
    //! The image in HSV format.
    cv::Mat m_hsvImage;
    //! The binary image after the color subtraction.
    cv::Mat m_differenceImage;
    //! The grayscale image.
    cv::Mat m_grayscaleImage;
    //! The binary image after threshold was applied.
    cv::Mat m_binaryImage;
    //! The foreground image.
    cv::Mat m_foregroundImage;
};


#endif // CATS2_COLOR_DETECTOR_HPP
