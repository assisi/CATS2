#include "FrameConvertor.hpp"

#include <QtGui/QImage>

const int FrameConvertor::TimeOutMs = 250;  // [ms]

/*!
* Constructor
*/
FrameConvertor::FrameConvertor(TimestampedFrameQueuePtr inputQueue) :
    QObject(),
    _inputQueue(inputQueue),
    _stopped(false)
{

}

/*!
* Destructor.
*/
FrameConvertor::~FrameConvertor()
{

}

/*!
 * Starts the convertor.
 */
void FrameConvertor::process()
{
    _stopped = false;
    TimestampedFrame frame;

    while (!_stopped) {
        // Use the blocking with timeout version of dequeue
        if (_inputQueue.data()->dequeue(frame, TimeOutMs))
            emit newFrame(cvMatToQImage(frame.image()));
    }

    emit finished();
}

/*!
 * Stops the convertor.
 */
void FrameConvertor::stop()
{
    _stopped = true;
}

/*!
 * Converts from openCV Mat to QImage.
 * Inspired by https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/
 */
QSharedPointer<QImage> FrameConvertor::cvMatToQImage(const QSharedPointer<cv::Mat>& imageCv)
{
    QImage* imageQt = nullptr;
    switch (imageCv.data()->type()) {
        case CV_8UC4: // 8-bit, 4 channel
        {
            imageQt = new QImage(imageCv.data()->data, imageCv.data()->cols, imageCv.data()->rows, imageCv.data()->step, QImage::Format_RGB32);
            break;
        }
        case CV_8UC3: // 8-bit, 3 channel
        {
            imageQt = new QImage(imageCv.data()->data, imageCv.data()->cols, imageCv.data()->rows, imageCv.data()->step, QImage::Format_RGB888);
            break;
        }
        default: // unsupported format
            imageQt = new QImage(imageCv.data()->cols, imageCv.data()->rows, QImage::Format_RGB888);
            imageQt->fill(Qt::black);
            break;
    }

    return  QSharedPointer<QImage>(imageQt);
}

