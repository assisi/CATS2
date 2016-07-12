#include "TimestampedFrame.hpp"

const int TimestampedFrameQueue::TimeOutMs = 250;  // [ms]


/*!
 * Constructor.
 */
TimestampedFrame::TimestampedFrame(cv::Mat* image, std::chrono::milliseconds timestamp):
    _image(image),
    _timestamp(timestamp)
{

}

/*!
 * Destructor.
 */
TimestampedFrame::~TimestampedFrame()
{

}
