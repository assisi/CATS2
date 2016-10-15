#include "BlobDetector.hpp"

#include <opencv2/bgsegm.hpp>

#include "settings/BlobDetectorSettings.hpp"
#include <TimestampedFrame.hpp>
#include <AgentData.hpp>

#include <QtCore/QtMath>
#include <QtCore/QMutexLocker>

/*!
 * Constructor. Gets the settings, the input queue to process and a queue to place debug images on request.
 */
BlobDetector::BlobDetector(TrackingRoutineSettingsPtr settings, TimestampedFrameQueuePtr inputQueue, TimestampedFrameQueuePtr debugQueue) :
    TrackingRoutine(inputQueue, debugQueue),
    m_backgroundCalculationStepCounter(0),
    m_backgroundSubtractor(cv::bgsegm::createBackgroundSubtractorMOG(200, 5, 0.3))
{
    // HACK : to get parameters specific for this tracker we need to convert the settings to the corresponding format
    BlobDetectorSettings* blobDetectorSettings = dynamic_cast<BlobDetectorSettings*>(settings.data());
    if (blobDetectorSettings != nullptr){
        // copy the parameters
        m_settings = blobDetectorSettings->data();
    } else {
        qDebug() << Q_FUNC_INFO << "Could not set the routune's settings";
    }

    // set the agents' list
    for (unsigned char id = 1; id <= m_settings.numberOfAgents(); id++) {
        AgentDataImage agent(QString::number(id), AgentType::GENERIC);
        m_agents.append(agent);
    }
}

/*!
 * Destructor.
 */
BlobDetector::~BlobDetector()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
}

/*!
 * The tracking routine excecuted. Gets the frame, tracks agents and fills the
 * _agents list, also sends out the debug images on request.
 * All the processing is done on the red channel of the original image.
 */
void BlobDetector::doTracking(const TimestampedFrame& frame)
{
    cv::Mat image = frame.image();

    // convert the image to grayscale
    cv::cvtColor(image, m_grayscaleImage, CV_RGB2GRAY);
    // learn the background
    if (m_backgroundCalculationStepCounter < BackgroundCalculationSufficientNumber) {
        // update the background
        m_backgroundSubtractor.get()->apply(m_grayscaleImage, m_foregroundImage, 0.05);
        m_backgroundCalculationStepCounter++;
        // until we have a solid background model it's pointless to do processing
        return;
    }
    // subract the background
    m_backgroundSubtractor.get()->apply(m_grayscaleImage, m_foregroundImage, 0.0);


    // dilate and erode directly after
    int an = 1;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(an*2+1, an*2+1), cv::Point(an, an));
    cv::dilate(m_foregroundImage, m_foregroundImage, element);
    cv::erode(m_foregroundImage, m_foregroundImage, element);

    // lock the mutex
    m_settingsMutex.lock();
    // filter out small blobs on the mask image
    if (m_settings.minBlobSizePx() > 0)
    {
        removeSmallBlobs(m_foregroundImage, m_settings.minBlobSizePx());
    }

    // all the corners corresponding to fishes' heads
    std::vector<cv::Point2f> corners;
    // centers of the detected objects
    std::vector<cv::Point2f> centers;
    // corners that are inside the detected objects
    std::vector<std::vector<cv::Point2f>> cornersInContours;

    try {
        // find the most prominent corners in the image
        cv::goodFeaturesToTrack(m_grayscaleImage,               // input
                                corners,                      // output
                                m_settings.numberOfAgents(),    // max number of corners to search
                                m_settings.qualityLevel(),
                                m_settings.minDistance(),
                                m_foregroundImage,              // mask
                                m_settings.blockSize(),
                                m_settings.useHarrisDetector(),
                                m_settings.k());
    } catch(cv::Exception& e) {
        qDebug() << Q_FUNC_INFO << "OpenCV exception: " << e.what();
    }

    // unlock the mutex
    m_settingsMutex.unlock();

    // find the contours in the image containing the detected corners
    detectContours(m_foregroundImage, corners, centers, cornersInContours);

    // submit the debug image
    if (m_enqueueDebugFrames) {
        enqueueDebugImage(m_foregroundImage);
    }

    // compute the agents orientations
    std::vector<float> directions(centers.size());
    for (size_t i = 0; i < directions.size(); ++i) {
        // FIXME : clean this logics of using only one contour
        cv::Point2f head = cornersInContours[i][0];
        cv::Point2f center = centers[i];
        directions[i] = qAtan2(head.y - center.y, head.x - center.x);
    }

    // tracking : assign the detected agents to id's
    assingIds(IdsAssignmentMethod::NAIVE_CLOSEST_NEIGHBOUR, centers, directions);
}

/*!
 * Finds and removes the blobs on the mask image that are smaller than given threshold (in px).
 */
void BlobDetector::removeSmallBlobs(cv::Mat& image, int minSize)
{
    // first get all the contours on the image
    std::vector<cv::Vec4i> hierarchy;
    std::vector<std::vector<cv::Point>> contours;

    cv::Mat contoursImage;  // used by the contours detection method cv::findContours();
                            // this methods draws the found contours directly on the
                            // source image thus modifying it.

    image.copyTo(contoursImage);
    try {
        cv::findContours(contoursImage, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    } catch(cv::Exception& e) {
        qDebug() << Q_FUNC_INFO << "OpenCV exception: " << e.what();
    }

    // check them one by one to find those that are smaller than minSize
    std::vector<std::vector<cv::Point>> smallContours;
    for (auto& contour : contours)
    {
        if (cv::contourArea(contour) < minSize)
        {
            // keep small contours to remove them later
            smallContours.push_back(contour);
        }
    }

    // fill small contours with black color
    if (smallContours.size() > 0)
    {
        // the line thikness -1 apparently makes this method to fill the contour
        // "https://www.researchgate.net/post/How_does_one_fill_a_closed_contour"
        cv::drawContours(image, smallContours, -1, cv::Scalar(0, 0, 0), -1);
    }
}

/*!
 * Finds the contours in the image.
 */
void BlobDetector::detectContours(cv::Mat& image,
                                  const std::vector<cv::Point2f>& corners,
                                  std::vector<cv::Point2f>& centers,
                                  std::vector<std::vector<cv::Point2f>>& cornersInContours)
{
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    try {
        // retrieve contours from the binary image
        cv::findContours(image, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    } catch(cv::Exception& e) {
        qDebug() << Q_FUNC_INFO << "OpenCV exception: " << e.what();
    }

    // draw the contours on the original image
    // TODO : rewrite it to fit the general style
    int idx = 0;
    cv::Scalar color(255, 255, 255);
    for (; idx >= 0; idx = hierarchy[idx][0] )
        cv::drawContours(image, contours, idx, color, CV_FILLED, 8, hierarchy);

    // Enclosing ellipses for detected objects
    std::vector<cv::RotatedRect> ellipses;
    std::vector<std::vector<cv::Point>> contoursPoly;
    contoursPoly.resize(contours.size());

    // contour's moment;
    cv::Moments moments;
    // all corners that are inside the contour
    std::vector<cv::Point2f> cornersInContour;

    for (size_t i = 0; i < contours.size(); ++i) {
        cv::approxPolyDP(cv::Mat(contours[i]), contoursPoly[i], 3, true);

        cornersInContour.clear();
        for (auto& corner: corners) {
            if(cv::pointPolygonTest(contours[i], corner, false) >= 0) {
                cornersInContour.push_back(corner);
            }
        }

        // take only the contours that contain corners
        if (cornersInContour.size() > 0) {
            moments = cv::moments(contours[i]);
            centers.push_back(cv::Point2f((float)(moments.m10/moments.m00+0.5),(float)(moments.m01/moments.m00+0.5)));
            cornersInContours.push_back(cornersInContour);
        }
    }

    // draw corners that are inside the contours and inclosing circles
    // and the corresponding countours' centers
    color = cv::Scalar(100, 100, 100);
    int r = 3;
    for (int i = 0; i < cornersInContours.size(); ++i) {
        cv::circle(image, cornersInContours[i][0], r, color, -1, 8, 0);
        cv::circle(image, centers[i], r, color, -1, 8, 0);
    }
}

/*!
 * Reports on what type of agent can be tracked by this routine.
 */
QList<AgentType> BlobDetector::capabilities() const
{
    return QList<AgentType>({AgentType::GENERIC});
}

/*!
 * Updates the settings.
 */
void BlobDetector::setSettings(const BlobDetectorSettingsData& settings)
{
    QMutexLocker locker(&m_settingsMutex);
    m_settings = settings;
}
