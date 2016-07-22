#include "BlobDetector.hpp"
#include "settings/BlobDetectorSettings.hpp"

/*!
 * Constructor. Gets the input queue to process and a queue to place debug images on request.
 */
BlobDetector::BlobDetector(TrackingRoutineSettingsPtr settings, TimestampedFrameQueuePtr inputQueue, TimestampedFrameQueuePtr debugQueue) :
    TrackingRoutine(inputQueue, debugQueue),
    m_backgroundCalculationStepCounter(0),
    m_backgroundSubtractor(cv::createBackgroundSubtractorMOG2(5))
{
    // HACK : to get parameters specific for this tracker we need to convert the settings to the corresponding format
    BlobDetectorSettings* blobDetectorSettings = dynamic_cast<BlobDetectorSettings*>(settings.data());
    if (blobDetectorSettings != nullptr){
        // copy the parameters
        m_settings = blobDetectorSettings->data();
    }

    // set the agents' list
    for (unsigned char id = 1; id <= m_settings.numberOfAgents(); id++) {
        AgentDataImage agent(id, AgentType::UNDEFINED);
        m_agents.append(agent);
    }
}

/*!
 * Destructor.
 */
BlobDetector::~BlobDetector()
{

}

/*!
 * The tracking routine excecuted. Gets the frame, tracks agents and fills the
 * _agents list, also sends out the debug images on request.
 * All the processing is done on the red channel of the original image.
 */
void BlobDetector::doTracking(const TimestampedFrame& frame)
{
    QSharedPointer<cv::Mat> image = frame.image();

    // get the red channel image
    std::vector<cv::Mat> channels;
    cv::split(*image.data(), channels);
    channels[0] = cv::Mat::zeros(image->rows, image->cols, CV_8UC1);  // blue channel is set to 0
    channels[1] = cv::Mat::zeros(image->rows, image->cols, CV_8UC1);  // green channel is set to 0
    cv::Mat redChannelImage;
    cv::merge(redChannelImage, channels);

    // get the background image
    cv::Mat redChannelImageWithoutBackground; // The mask image to be used by the feature detection.

    if (m_backgroundCalculationStepCounter < BackgroundCalculationSufficientNumber) {
        // update the background
        m_backgroundSubtractor.get()->apply(redChannelImage, redChannelImageWithoutBackground, 0.10);
        m_backgroundCalculationStepCounter++;
        // until we have a solid background model it's pointless to do processing
    } else {
        // subract the backgound
        m_backgroundSubtractor.get()->apply(redChannelImage, redChannelImageWithoutBackground, 0.0);

        // NOTE : Add an erosion after ?
        cv::dilate(redChannelImageWithoutBackground, redChannelImageWithoutBackground, cv::Mat(), cv::Point(-1, -1), 1);

        // filter out small blobs on the mask image
        if (m_settings.minBlobSizePx() > 0)
        {
            removeSmallBlobs(redChannelImageWithoutBackground, m_settings.minBlobSizePx());
        }

        // find the fishes on the stored red channel image
        std::vector<cv::Point2f> corners;
        cv::goodFeaturesToTrack(redChannelImage,
                                corners,
                                m_settings.numberOfAgents(),
                                m_settings.qualityLevel(),
                                m_settings.minDistance(),
                                redChannelImageWithoutBackground,
                                m_settings.blockSize(),
                                m_settings.useHarrisDetector(),
                                m_settings.k());


        // find the contours in the image
        cv::Mat maskImage;
        detectContours(redChannelImageWithoutBackground, corners);
    }
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
    try
    {
        cv::findContours(contoursImage, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    }
    catch(cv::Exception& e)
    {
        qDebug() << Q_FUNC_INFO << "OpenCV exception: " << e.what();
    }

    // check them one by one to find those that are smaller than minSize
    std::vector<std::vector<cv::Point>> smallContours;
    for(size_t i = 0; i < contours.size(); ++i)
    {
        if (cv::contourArea(contours[i]) < minSize)
        {
            // keep small contours to remove them later
            smallContours.push_back(contours[i]);
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
 * Finds the contours in the red channel image.
 */
void BlobDetector::detectContours(cv::Mat& image, std::vector<cv::Point2f>& corners)
{
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    try {
        cv::findContours(image, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    } catch(cv::Exception& e) {
        qDebug() << Q_FUNC_INFO << "OpenCV exception: " << e.what();
    }

    std::vector<std::vector<cv::Point>> contoursPoly;
    contoursPoly.resize(contours.size());

    m_boundRect.resize(contours.size());
    m_centers.resize(contours.size());
    m_radius.resize(contours.size());
    m_cornersInPoly.resize(contours.size());

    for(size_t i = 0; i < contours.size(); ++i) {
        cv::approxPolyDP(cv::Mat(contours[i]), contoursPoly[i], 3, true);
        m_boundRect[i] = cv::boundingRect(cv::Mat(contoursPoly[i]));
        cv::minEnclosingCircle(contoursPoly[i], m_centers[i], m_radius[i]);

        for(auto c: corners) {
            if(cv::pointPolygonTest(contours[i], c, false) >= 0) {
                m_cornersInPoly[i].push_back(c);
            }
        }
    }
}


/////////////////////////////////////////////////////////
///////////////////// AgentClassifier ///////////////////
///////////////////////////////////////////////////////// {{{1

/*

void AgentClassifier::_computeLocalIDs(BlobDetector& bDetector) {
    std::vector<size_t> remainingAgents(agents.size());
    for(size_t i = 0; i < remainingAgents.size(); ++i) remainingAgents[i] = i;
    double const minDistThreshold = 15.; // XXX min Dist threshold from config

    for(size_t i = 0; i < bDetector.centers.size(); ++i) {
        double minDist = 1000000.0; // XXX max double
        size_t indexMinDist = 0;
        bool detected = false;
        for(size_t j = 0; j < remainingAgents.size(); ++j) {
            auto const& a = agents[remainingAgents[j]];
            auto const& b = bDetector.centers[i];
            double const dist = sqrt((b.x - a.center.x) * (b.x - a.center.x) + (b.y - a.center.y) * (b.y - a.center.y));
            if(bDetector.cornersInPoly[i].size() && dist < minDist) {
                minDist = dist;
                indexMinDist = remainingAgents[j];
                detected = true;
            }
        }

        if(detected) {
            auto& a = agents[indexMinDist];
            a.head = bDetector.cornersInPoly[i][0];
            a.center = bDetector.centers[i];
            a.direction = atan2(a.head.y - a.center.y, a.head.x - a.center.x);
            a.detected = true;
            a.closeToOtherAgent = bDetector.cornersInPoly[i].size() > 1;
            //std::cout << "DEBUGa: " << i << " " << a.head << " " << a.center << std::endl;
            remainingAgents.erase(std::remove(remainingAgents.begin(), remainingAgents.end(), indexMinDist), remainingAgents.end()); // https://en.wikipedia.org/wiki/Erase–remove_idiom
            a.localIDAccurate = minDist < minDistThreshold;
        }
    }

    for(size_t j = 0; j < remainingAgents.size(); ++j) {
        auto& a = agents[remainingAgents[j]];
        a.detected = false;
        a.closeToOtherAgent = false;
        a.localIDAccurate = false;
    }

    // Determine if there are trackedAgents that are too close to each other
    for(size_t i = 0; i < agents.size(); ++i) {
        for(size_t j = i + 1; j < agents.size(); ++j) {
            if(!(agents[i].detected && agents[j].detected)) continue;
            double const distHead = sqrt((agents[i].head.x - agents[j].head.x) * (agents[i].head.x - agents[j].head.x) + (agents[i].head.y - agents[j].head.y) * (agents[i].head.y - agents[j].head.y));
            double const distCenter = sqrt((agents[i].center.x - agents[j].center.x) * (agents[i].center.x - agents[j].center.x) + (agents[i].center.y - agents[j].center.y) * (agents[i].center.y - agents[j].center.y));
            bool const close = distHead < 5.0 || distCenter < 5.0;
            agents[i].closeToOtherAgent |= close;
            agents[j].closeToOtherAgent |= close;
            //std::cout << "DEBUGc: " << i << "," << j << ": " << close << std::endl;
        }
    }

    // Assign local ID
    for(auto& a: agents) {
        if(a.detected) {
            if(a.closeToOtherAgent)
                a.localIDAccurate = false;
            if(!a.localIDAccurate) {
                a.localId = currentMaxId;
                ++currentMaxId;
                a.localIDAccurate = true;
            }
        }
        //std::cout << "DEBUGd: " << a.detected << " " << a.closeToOtherAgent << " " << a.localIDAccurate << " " << a.localId << std::endl;
    }
}


void AgentClassifier::_updateHistory() {
    for(auto const& a: agents) {
        if(!a.detected) continue;

        // Find (or create) local id descriptor
        auto const& localId = a.localId;
        if(!_localIds.count(localId)) {
            size_t const startingFrame = 0; // TODO
            LocalIdDescriptor descriptor(localId, startingFrame);
            _localIds.insert({localId, descriptor});
        }
        LocalIdDescriptor& descriptor = _localIds.at(localId);

        // Update history size (i.e. nb timesteps for each local ID
        ++descriptor.histSize;

        // Update mean Agent Size
        descriptor.histAgentSize.push_back(a.size());
    }
}


void AgentClassifier::_identifyAgentsType() {
    // TODO
}


void AgentClassifier::localIdentification(BlobDetector& bDetector) {
    _computeLocalIDs(bDetector);
    _updateHistory(); // XXX
    _identifyAgentsType(); // XXX
}



*/
