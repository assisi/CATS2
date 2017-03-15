#include "ColorDetector.hpp"

#include "settings/ColorDetectorSettings.hpp"
#include <TimestampedFrame.hpp>
#include <AgentData.hpp>

#include <opencv2/bgsegm.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include <QtCore/QtMath>
#include <QtCore/QMutexLocker>

/*!
 * Constructor. Gets the settings, the input queue to process and a queue to place debug images on request.
 */
ColorDetector::ColorDetector(TrackingRoutineSettingsPtr settings, TimestampedFrameQueuePtr inputQueue, TimestampedFrameQueuePtr debugQueue) :
    TrackingRoutine(inputQueue, debugQueue)
{
    // HACK : to get parameters specific for this tracker we need to convert the settings to the corresponding format
    ColorDetectorSettings* colorDetectorSettings = dynamic_cast<ColorDetectorSettings*>(settings.data());
    if (colorDetectorSettings != nullptr){
        // copy the parameters
        m_settings = colorDetectorSettings->data();
    } else {
        qDebug() << Q_FUNC_INFO << "Could not set the routune's settings";
    }

    // set the mask file
    m_maskImage = cv::imread(m_settings.maskFilePath()/*, cv::IMREAD_GRAYSCALE*/);

    // set the agents' list
    for (unsigned char id = 1; id <= m_settings.numberOfAgents(); id++) {
        AgentDataImage agent(QString::number(id), AgentType::GENERIC);
        m_agents.append(agent);
    }
}

/*!
 * Destructor.
 */
ColorDetector::~ColorDetector()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
}

/*!
 * The tracking routine excecuted. Gets the frame, tracks agents and fills the
 * _agents list, also sends out the debug images on request.
  */
void ColorDetector::doTracking(const TimestampedFrame& frame)
{
    cv::Mat image = frame.image();

    // limit the image format to three channels color images
    if (image.type() == CV_8UC3) {
        // first blur the image
        cv::blur(image, m_blurredImage, cv::Size(3, 3));

        // apply the mask if defined
        // TODO : to add support for different types of masks to be less picky
        if ((m_maskImage.data != nullptr) && (m_blurredImage.type() == m_maskImage.type()))
            m_blurredImage = m_blurredImage & m_maskImage;
        else {
//            qDebug() << Q_FUNC_INFO << "The mask's type is not compatible with the image";
        }

        int h,s,v;
        m_settingsMutex.lock();
        m_settings.color().getHsv(&h, &s, &v);
        int tolerance = m_settings.threshold();
        m_settingsMutex.unlock();

        // convert to hsv
        cv::cvtColor(m_blurredImage, m_hsvImage, CV_RGB2HSV);
        // threshold the image in the HSV color space
        cv::inRange(m_hsvImage,
                    cv::Scalar(h / 2 - tolerance, 0 , v - 2 * tolerance),
                    cv::Scalar(h / 2 + tolerance, 255, 255),
                    m_binaryImage); //

        // postprocessing of the binary image
        int an = 1;
        cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(an*2+1, an*2+1), cv::Point(an, an)); // TODO : inititialize this in the constructor


        //morphological opening (remove small objects from the foreground)
        cv::erode(m_binaryImage, m_binaryImage, element);
        cv::dilate(m_binaryImage, m_binaryImage, element);

        //morphological closing (fill small holes in the foreground)
        cv::dilate(m_binaryImage, m_binaryImage, element);
        cv::erode(m_binaryImage, m_binaryImage, element);

//        // submit the debug image
//        if (m_enqueueDebugFrames) {
//            enqueueDebugImage(m_binaryImage);
//        }
        // TODO : make a devoted method
        // detect the spots as contours
        std::vector<std::vector<cv::Point>> contours;
        try { // TODO : to check if this try-catch can be removed or if it should be used everywhere where opencv methods are used.
            // retrieve contours from the binary image
            cv::findContours(m_binaryImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        } catch(cv::Exception& e) {
            qDebug() << Q_FUNC_INFO << "OpenCV exception: " << e.what();
        }

        // sort the contours by size (inspired by http://stackoverflow.com/questions/33401745/find-largest-contours-opencv)
        std::vector<int> indices(contours.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::sort(indices.begin(), indices.end(), [&contours](int lhs, int rhs) {
            return cv::contourArea(contours[lhs],false) > cv::contourArea(contours[rhs],false);
        });

        // centers of contour
        int agentIndex = 0;
        std::vector<cv::Point2f> centers;
        cv::Moments moments;
        for (auto& contour: contours) {
            moments = cv::moments(contour);
            if (agentIndex < m_agents.size()) {
                m_agents[agentIndex].mutableState()->setPosition(cv::Point2f(static_cast<float>(moments.m10/moments.m00+0.5),
                                                                             static_cast<float>(moments.m01/moments.m00+0.5)));
                agentIndex++;
            } else {
                break;
            }
        }

        // submit the debug image
        if (m_enqueueDebugFrames) {
            for (auto& agent: m_agents) {
                cv::circle(m_blurredImage, cv::Point(agent.state().position().x(), agent.state().position().y()), 2, cv::Scalar(255, 255, 255));
            }
            enqueueDebugImage(m_blurredImage);
        }
    }
    else
        qDebug() << Q_FUNC_INFO << "Unsupported image format" << image.type();
}

/*!
 * Reports on what type of agent can be tracked by this routine.
 */
QList<AgentType> ColorDetector::capabilities() const
{
    return QList<AgentType>({AgentType::CASU}); // FIXME : must be generic
}

/*!
 * Updates the settings.
 */
void ColorDetector::setSettings(const ColorDetectorSettingsData& settings)
{
    QMutexLocker locker(&m_settingsMutex);
    m_settings = settings;
}
