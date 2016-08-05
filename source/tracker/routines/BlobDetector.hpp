#ifndef CATS2_BLOB_DETECTOR_HPP
#define CATS2_BLOB_DETECTOR_HPP

#include "TrackingRoutine.hpp"
#include "settings/BlobDetectorSettings.hpp"
#include "TrackerPointerTypes.hpp"

class BlobDetector : public TrackingRoutine
{
    Q_OBJECT
public:
    //! Constructor. Gets the input queue to process and a queue to place debug images on request.
    explicit BlobDetector(TrackingRoutineSettingsPtr settings, TimestampedFrameQueuePtr inputQueue, TimestampedFrameQueuePtr debugQueue);
    //! Destructor.
    virtual ~BlobDetector();

protected:
    //! The tracking routine excecuted. Gets the original frame, detects
    //! agents, eventually associates them with the trajectories and
    //! enqueue debug images on request. Overriden from TrackingRoutine.
    virtual void doTracking(const TimestampedFrame& frame) override;

private:
    //! Finds and removes the blobs on the mask image that are smaller than given threshold (in px).
    void removeSmallBlobs(cv::Mat& image, int minSize);
    //! Finds the contours in the red channel image.
    void detectContours(cv::Mat& image, std::vector<cv::Point2f>& corners);

    //    void getAgentsImage();

private:
    //! First processing steps are used to compute the background.
    size_t m_backgroundCalculationStepCounter;
    //! The number of steps that is enough to compute the background.
    static const size_t BackgroundCalculationSufficientNumber = 120;
    //! The tracking settings.
    BlobDetectorSettingsData m_settings;

private:

    //! The backgound model.
    cv::Ptr<cv::BackgroundSubtractor> m_backgroundSubtractor;
    //! The tracking results.
    std::vector<cv::Rect> m_boundRect;
    std::vector<cv::Point2f> m_centers;
    std::vector<float> m_radius;
    std::vector<std::vector<cv::Point2f>> m_cornersInPoly;


//    //!
//    short _bgNbMixtures;

//    cv::Mat _img;
//    cv::Mat const* _imgOrig = nullptr;


//    cv::Mat _maskOrig;

//private:
//    std::vector<cv::Mat> agentsImg;


};

#endif // CATS2_BLOB_DETECTOR_HPP
