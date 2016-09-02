#include "FrameConvertor.hpp"

#include <TimestampedFrame.hpp>

#include <QtGui/QImage>
#include <QtGui/QPixmap>
#include <QtCore/QtMath>

/*!
* Constructor.
*/
FrameConvertor::FrameConvertor(TimestampedFrameQueuePtr inputQueue) :
    QObject(),
    m_inputQueue(inputQueue),
    m_stopped(false),
    m_previousTimestamp(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()))
{
    qRegisterMetaType<QSharedPointer<QImage>>("QSharedPointer<QPixmap>");
}

/*!
* Destructor.
*/
FrameConvertor::~FrameConvertor()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
}

/*!
 * Starts the convertor.
 */
void FrameConvertor::process()
{
    if (!m_inputQueue.isNull()) {
        m_stopped = false;
        TimestampedFrame frame;

        while (!m_stopped) {
            // Use the blocking with timeout version of dequeue
            if (m_inputQueue->dequeue(frame)) {
                // compute the frame rate
                std::chrono::milliseconds timeFromPreviousFrameMs = frame.timestamp() - m_previousTimestamp;
                m_previousTimestamp = frame.timestamp();
                // send the data
                emit newFrame(cvMatToQPixmap(frame.image()), qFloor(1000./timeFromPreviousFrameMs.count()) + 0.5);
            }
        }
    } else {
        qDebug() << Q_FUNC_INFO << "Input queue is not set, finishing.";
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
QSharedPointer<QPixmap> FrameConvertor::cvMatToQPixmap(const cv::Mat& imageCv)
{
    QImage* imageQt;
    QPixmap* pixmapQt = nullptr;
    switch (imageCv.type()) {
        case CV_8UC4: // 8-bit, 4 channel
        {
            imageQt = new QImage(imageCv.data, imageCv.cols, imageCv.rows, imageCv.step, QImage::Format_RGB32);
            break;
        }
        case CV_8UC3: // 8-bit, 3 channel
        {
            imageQt = new QImage(imageCv.data, imageCv.cols, imageCv.rows, imageCv.step, QImage::Format_RGB888);
            break;
        }
        case CV_8UC1: // 8-bit, 1 channel
        {
            imageQt = new QImage(imageCv.data, imageCv.cols, imageCv.rows, imageCv.step, QImage::Format_Grayscale8);
            break;
        }
        default: // unsupported format
            imageQt = new QImage(imageCv.cols, imageCv.rows, QImage::Format_RGB888);
            imageQt->fill(Qt::black);
            break;
    }

    pixmapQt = new QPixmap(QPixmap::fromImage(*imageQt));

    return  QSharedPointer<QPixmap>(pixmapQt);
}

