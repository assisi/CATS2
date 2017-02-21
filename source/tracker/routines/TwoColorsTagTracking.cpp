#include "TwoColorsTagTracking.hpp"

#include "settings/TwoColorsTagTrackingSettings.hpp"
#include <TimestampedFrame.hpp>
#include <AgentData.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <QtCore/QtMath>
#include <QtCore/QMutex>

#include <cmath>

/*!
 * Constructor. Gets the settings, the input queue to process and a queue to
 * place debug images on request.
 */
TwoColorsTagTracking::TwoColorsTagTracking(TrackingRoutineSettingsPtr settings,
                                           TimestampedFrameQueuePtr inputQueue,
                                           TimestampedFrameQueuePtr debugQueue) :
    TrackingRoutine(inputQueue, debugQueue)
{
    // HACK : to get parameters specific for this tracker we need to convert
    // the settings to the corresponding format
    TwoColorsTagTrackingSettings* twoColorsTagTrackingSettings =
            dynamic_cast<TwoColorsTagTrackingSettings*>(settings.data());
    if (twoColorsTagTrackingSettings != nullptr){
        // copy the parameters
        m_settings = twoColorsTagTrackingSettings->data();
    } else {
        qDebug() << Q_FUNC_INFO << "Could not set the routune's settings";
    }

    // set the agents' list consisting from one agent only
    AgentDataImage agent("T", AgentType::GENERIC);
    m_agents.append(agent);
}

/*!
 * Destructor.
 */
TwoColorsTagTracking::~TwoColorsTagTracking()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
}

/*!
 * The tracking routine excecuted.
 */
void TwoColorsTagTracking::doTracking(const TimestampedFrame& frame)
{
    cv::Mat image = frame.image();

    // limit the image format to three channels color images
    if (image.type() == CV_8UC3) {
        // save the debug image
        image.copyTo(m_debugImage);
        // first blur the image
        cv::blur(image, m_blurredImage, cv::Size(3, 3));

        // detect tags
        cv::Point2f frontTagCenter;
        cv::Point2f backTagCenter;
        if (detectTags(TwoColorsTagTrackingSettingsData::TagType::FRONT, frontTagCenter) &&
                detectTags(TwoColorsTagTrackingSettingsData::TagType::BACK, backTagCenter))
        {
            // compute the orientation
            double orientation = qAtan2(frontTagCenter.y - backTagCenter.y,
                                        frontTagCenter.x - backTagCenter.x);
            // get the position
            double coef = m_settings.centerProportionalPosition();
            cv::Point2f center = frontTagCenter * (1 - coef) + backTagCenter * coef;
            if (m_agents.size() > 0) {
                m_agents[0].mutableState()->setPosition(center);
                m_agents[0].mutableState()->setOrientation(orientation);
            }
            // submit the debug image
            if (m_enqueueDebugFrames) {
                cv::Scalar color(255, 255, 255);
                cv::circle(m_debugImage, center, 2, color);
                cv::arrowedLine(m_debugImage, center, frontTagCenter, color);
                enqueueDebugImage(m_debugImage);
            }
        }

    }
    else
        qDebug() << Q_FUNC_INFO << "Unsupported image format" << image.type();
}

/*!
 * Searches for the given tag group, returns it's centre of mass.
*/
bool TwoColorsTagTracking::detectTags(TwoColorsTagTrackingSettingsData::TagType tagType,
                                      cv::Point2f& tagGroupCenter)
{

    TwoColorsTagTrackingSettingsData::TagGroupDescription description;
    {
        QMutexLocker locker(&m_settingsMutex);
        description = m_settings.tagGroupDescription(tagType);
    }
    int h,s,v;
    description.tagColor.getHsv(&h, &s, &v);

    // convert to hsv
    cv::cvtColor(m_blurredImage, m_hsvImage, CV_RGB2HSV);
    // threshold the image in the HSV color space
    cv::inRange(m_hsvImage,
                cv::Scalar(h / 2 - description.colorThreshold, 0 ,
                           v - 2 * description.colorThreshold),
                cv::Scalar(h / 2 + description.colorThreshold, 255, 255),
                m_binaryImage);

    // postprocessing of the binary image
    int an = 2;
    // TODO : inititialize this in the constructor
    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                                cv::Size(an*2+1, an*2+1),
                                                cv::Point(an, an));

    //morphological opening (remove small objects from the foreground)
    cv::erode(m_binaryImage, m_binaryImage, element);
    cv::dilate(m_binaryImage, m_binaryImage, element);

    //morphological closing (fill small holes in the foreground)
    cv::dilate(m_binaryImage, m_binaryImage, element);
    cv::erode(m_binaryImage, m_binaryImage, element);

    //    if (tagType == TwoColorsTagTrackingSettingsData::TagType::BACK) {
    //        // submit the debug image
    //        if (m_enqueueDebugFrames) {
    //            enqueueDebugImage(m_binaryImage);
    //        }
    //    }

    // detect the blobs as contours
    std::vector<std::vector<cv::Point>> contours;
    try { // TODO : to check if this try-catch can be removed or if it should be used everywhere where opencv methods are used.
        // retrieve contours from the binary image
        cv::findContours(m_binaryImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    } catch(cv::Exception& e) {
        qDebug() << Q_FUNC_INFO << "OpenCV exception: " << e.what();
    }

    // sort the contours to find biggest
    std::vector<int> indices(contours.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&contours](int lhs, int rhs) {
        return cv::contourArea(contours[lhs]) > cv::contourArea(contours[rhs]);
    });

    // if we detected all the tags
    if (contours.size() >= description.numberOfTags) {
        // center of the contour
        tagGroupCenter = cv::Point2f(0, 0);
        for (size_t i = 0; i < description.numberOfTags; ++i) {
            tagGroupCenter += contourCenter(contours[i]);
        }
        tagGroupCenter /= description.numberOfTags;

        // submit the debug image
        if (m_enqueueDebugFrames) {
            cv::Scalar color(255, 255, 255);
            for (size_t i = 0; i < description.numberOfTags; ++i) {
                cv::circle(m_debugImage, contourCenter(contours[i]), 2, color);
            }
        }
        return true;
    }


    return false;
}

/*!
 * Reports on what type of agent can be tracked by this routine.
 */
QList<AgentType> TwoColorsTagTracking::capabilities() const
{
    return QList<AgentType>({AgentType::GENERIC});
}

/*!
 * Updates the settings from the GUI.
 */
void TwoColorsTagTracking::setSettings(const TwoColorsTagTrackingSettingsData& settings)
{
    QMutexLocker locker(&m_settingsMutex);
    m_settings = settings;
}
