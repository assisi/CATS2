#include "FrameConvertor.hpp"

#include <TimestampedFrame.hpp>

#include <QtGui/QImage>

/*!
* Constructor.
*/
FrameConvertor::FrameConvertor(TimestampedFrameQueuePtr inputQueue) :
    QObject(),
    m_inputQueue(inputQueue),
    m_stopped(false)
{
    qRegisterMetaType<QSharedPointer<QImage>>("QSharedPointer<QImage>");
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
    m_stopped = false;
    TimestampedFrame frame;

    while (!m_stopped) {
        // Use the blocking with timeout version of dequeue
        if (m_inputQueue->dequeue(frame))
            emit newFrame(cvMatToQImage(frame.image()));
    }

    emit finished();
}

/*!
 * Stops the convertor.
 */
void FrameConvertor::stop()
{
    m_stopped = true;
}

/*!
 * Converts from openCV Mat to QImage.
 * Inspired by https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/
 */
QSharedPointer<QImage> FrameConvertor::cvMatToQImage(const QSharedPointer<cv::Mat>& imageCv)
{
    QImage* imageQt = nullptr;
    switch (imageCv->type()) {
        case CV_8UC4: // 8-bit, 4 channel
        {
            imageQt = new QImage(imageCv->data, imageCv->cols, imageCv->rows, imageCv->step, QImage::Format_RGB32);
            break;
        }
        case CV_8UC3: // 8-bit, 3 channel
        {
            imageQt = new QImage(imageCv->data, imageCv->cols, imageCv->rows, imageCv->step, QImage::Format_RGB888);
            break;
        }
        default: // unsupported format
            imageQt = new QImage(imageCv->cols, imageCv->rows, QImage::Format_RGB888);
            imageQt->fill(Qt::black);
            break;
    }

    return  QSharedPointer<QImage>(imageQt);
}

