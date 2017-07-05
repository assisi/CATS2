#ifndef CATS2_BLOB_DETECTOR_HPP
#define CATS2_BLOB_DETECTOR_HPP

#include "TrackingRoutine.hpp"
#include "settings/BlobDetectorSettings.hpp"
#include "TrackerPointerTypes.hpp"

#include <QtCore/QMutex>

/*!
 * A tracker inherited from the original CATS software. It uses the corner detection to find agents in the image.
 */
class BlobDetector : public TrackingRoutine
{
    Q_OBJECT
public:
    //! Constructor. Gets the settings, the input queue to process and a queue to place debug images on request.
    explicit BlobDetector(TrackingRoutineSettingsPtr settings, TimestampedFrameQueuePtr inputQueue, TimestampedFrameQueuePtr debugQueue);
    //! Destructor.
    virtual ~BlobDetector();

    //! Reports on what type of agent can be tracked by this routine.
    virtual QList<AgentType> capabilities() const override;

    //! Getter for the settings.
    const BlobDetectorSettingsData& settings() const { return m_settings; }
    //! Updates the settings.
    void setSettings(const BlobDetectorSettingsData& settings);
    //! Reset the background.
    void resetBackground();

protected:
    //! The tracking routine excecuted. Gets the original frame, detects
    //! agents, eventually associates them with the trajectories and
    //! enqueue debug images on request. Overriden from TrackingRoutine.
    virtual void doTracking(const TimestampedFrame& frame) override;

private:
    //! Finds and removes the blobs on the mask image that are smaller than given threshold (in px).
    void removeSmallBlobs(cv::Mat& image, int minSize);
    //! Finds the contours in the red channel image.
    void detectContours(cv::Mat& image, const std::vector<cv::Point2f>& m_corners, std::vector<cv::Point2f>& centers, std::vector<std::vector<cv::Point2f>>& cornersInContours);

private:
    //! First processing steps are used to compute the background.
    std::atomic_size_t m_backgroundCalculationStepCounter;
    //! The number of steps that is enough to compute the background.
    static const size_t BackgroundCalculationSufficientNumber = 100;
    //! The tracking settings.
    BlobDetectorSettingsData m_settings;

private:
    //! The backgound model.
    cv::Ptr<cv::BackgroundSubtractor> m_backgroundSubtractor;
    //! The intermediate data.
    //! The grayscale version of the frame image.
    cv::Mat m_grayscaleImage;
    //! The foreground image.
    cv::Mat m_foregroundImage;
};

#endif // CATS2_BLOB_DETECTOR_HPP
