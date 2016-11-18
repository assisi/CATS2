#include "FishBotLedsTracking.hpp"

#include "settings/FishBotLedsTrackingSettings.hpp"
#include <TimestampedFrame.hpp>
#include <AgentData.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <QtCore/QtMath>
#include <QtCore/QMutex>

#include <cmath>

/*!
 * Constructor. Gets the settings, the input queue to process and a queue to place debug images on request.
 */
FishBotLedsTracking::FishBotLedsTracking(TrackingRoutineSettingsPtr settings, TimestampedFrameQueuePtr inputQueue, TimestampedFrameQueuePtr debugQueue) :
    TrackingRoutine(inputQueue, debugQueue)
{
    // HACK : to get parameters specific for this tracker we need to convert the settings to the corresponding format
    FishBotLedsTrackingSettings* fishBotLedsTrackingSettings = dynamic_cast<FishBotLedsTrackingSettings*>(settings.data());
    if (fishBotLedsTrackingSettings != nullptr){
        // copy the parameters
        m_settings = fishBotLedsTrackingSettings->data();
    } else {
        qDebug() << Q_FUNC_INFO << "Could not set the routune's settings";
    }

    // set the mask file
    m_maskImage = cv::imread(m_settings.maskFilePath()/*, cv::IMREAD_GRAYSCALE*/);

    // set the agents' list
    FishBotLedsTrackingSettingsData::FishBotDescription robotDescription;
    for (size_t robotIndex = 0; robotIndex < m_settings.numberOfAgents(); robotIndex++) {
        robotDescription = m_settings.robotDescription(robotIndex);
        AgentDataImage agent(robotDescription.id, AgentType::FISH_CASU);
        m_agents.append(agent);
    }

    // initialize the previous states
    for (const auto& agent : m_agents)
        m_previousStates.append(agent.state());
}

/*!
 * Destructor.
 */
FishBotLedsTracking::~FishBotLedsTracking()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
}

/*!
 * The tracking routine excecuted.
 */
void FishBotLedsTracking::doTracking(const TimestampedFrame& frame)
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

        // detect robots
        m_settingsMutex.lock();
        unsigned int numberOfAgents = m_settings.numberOfAgents();
        m_settingsMutex.unlock();
        for (size_t robotIndex = 0; robotIndex < numberOfAgents; robotIndex++) {
            detectLeds(robotIndex);
        }

        // copy the states for the next iteration
        m_previousStates.clear();
        for (const auto& agent : m_agents)
            m_previousStates.append(agent.state());

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
 * Searches for the given robot's leds on the image.
*/
void FishBotLedsTracking::detectLeds(size_t robotIndex)
{
    int h,s,v;
    m_settingsMutex.lock();
    m_settings.robotDescription(robotIndex).ledColor.getHsv(&h, &s, &v);
    int tolerance = m_settings.robotDescription(robotIndex).colorThreshold;
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

    // TODO : make a devoted method
    // detect the leds as contours, normally only two should be found
    std::vector<std::vector<cv::Point>> contours;
    try { // TODO : to check if this try-catch can be removed or if it should be used everywhere where opencv methods are used.
        // retrieve contours from the binary image
        cv::findContours(m_binaryImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    } catch(cv::Exception& e) {
        qDebug() << Q_FUNC_INFO << "OpenCV exception: " << e.what();
    }

    // sort the contours to find two biggest (inspired by http://stackoverflow.com/questions/33401745/find-largest-contours-opencv)
    std::vector<int> indices(contours.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&contours](int lhs, int rhs) {
        return cv::contourArea(contours[lhs],false) > cv::contourArea(contours[rhs],false);
    });

    // current agent
    AgentDataImage& robot = m_agents[robotIndex];
    // previous state
    StateImage& previousState = m_previousStates[robotIndex];

    // if the size is correct then get two biggest contours
    cv::Point2f agentPosition;
    double agentOrientation;
    if (contours.size() > 1) {
        // center of the contour
        std::vector<cv::Point2f> contourCenters;
        // contour's moments
        cv::Moments moments;
        for (size_t i = 0; i < 2; ++i) {
            moments = cv::moments(contours[i]);
            contourCenters.push_back(cv::Point2f(static_cast<float>(moments.m10/moments.m00+0.5),
                                                 static_cast<float>(moments.m01/moments.m00+0.5)));
        }
        // compute the agent's position that is between two contours, and the orientation
        agentPosition = ((contourCenters[0] + contourCenters[1]) / 2);
        // this orientation is still precise up to +180 degrees
        agentOrientation = qAtan2(contourCenters[1].y - contourCenters[0].y,
                                  contourCenters[1].x - contourCenters[0].x);
        // define the direction
        cv::Point2f agentVector = contourCenters[1] - contourCenters[0]; // the agent body
        cv::Point2f agentDisplacementVector = agentPosition - previousState.position().toCvPoint2f(); // new postion minus previous position
        // first we define the orientation with the displacement, 0.3 px is an empirical parameter to
        // decide that the robot moves
         if (previousState.position().isValid() && (cv::norm(agentDisplacementVector) > 0.30)) {
            // if vectors are oppositely directed then we correct the orientation
            if (agentVector.dot(agentDisplacementVector) < 0)
                agentOrientation += M_PI;
            robot.mutableState()->setOrientation(agentOrientation);
        } // FIXME : debug this part asap
         /*else if (previousState.orientation().isValid()) {
            // otherwise we try to define the correct orientation with the previous orientation
            cv::Point2f previousAgentUnitVector(qCos(previousState.orientation().angleRad()),
                                                qSin(previousState.orientation().angleRad()));
            // if vectors are oppositely directed then we correct the orientation
            if (agentVector.dot(previousAgentUnitVector) < 0)
                agentOrientation += M_PI;
            robot.mutableState()->setOrientation(agentOrientation);
        } */else {
            // the orientation can not be defined
            robot.mutableState()->setOrientation(agentOrientation);
            robot.mutableState()->invalidateOrientation();
        }

         // set the position
        robot.mutableState()->setPosition(agentPosition);
        robot.setTimestamp(m_currentTimestamp);
    } else if (contours.size() == 1){
        // if only one blob is detected, then we take its position as the robot's position
        cv::Moments moments = cv::moments(contours[0]);
        agentPosition = cv::Point2f(static_cast<float>(moments.m10/moments.m00+0.5),
                                    static_cast<float>(moments.m01/moments.m00+0.5));
        robot.mutableState()->setPosition(agentPosition);
        robot.setTimestamp(m_currentTimestamp);
    } else {
        // TODO : add a Kalman here to avoid loosing the robot when sometimes
        // it's hidden by other objects on the arena.
    }
}

/*!
 * Reports on what type of agent can be tracked by this routine.
 */
QList<AgentType> FishBotLedsTracking::capabilities() const
{
    return QList<AgentType>({AgentType::FISH_CASU});
}

/*!
 * Updates the settings.
 */
void FishBotLedsTracking::setSettings(const FishBotLedsTrackingSettingsData& settings)
{
    QMutexLocker locker(&m_settingsMutex);
    m_settings = settings;
}
