#include "TimestampedFrame.hpp"

const int TimestampedFrameQueue::TimeOutMs = 250;  // [ms]


/*!
 * Constructor.
 */
TimestampedFrame::TimestampedFrame(cv::Mat* image, std::chrono::milliseconds timestamp):
    m_image(image),
    m_timestamp(timestamp)
{

}

/*!
 * Destructor.
 */
TimestampedFrame::~TimestampedFrame()
{

}
