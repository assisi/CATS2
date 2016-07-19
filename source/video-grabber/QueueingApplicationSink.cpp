#include "QueueingApplicationSink.hpp"

#include <gst/gstcaps.h>
#include <gst/video/video.h>

#include <QGst/buffer.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QtGui/QImage>

#include <chrono>

/*!
 * Constructor. Gets a queue to put the frames in.
 */
QueueingApplicationSink::QueueingApplicationSink(TimestampedFrameQueuePtr outputQueue):
     QGst::Utils::ApplicationSink(),
    _outputQueue(outputQueue)
{
}

/*!
 * Called when a new sample arrives.
 */
QGst::FlowReturn QueueingApplicationSink::newBuffer()
{
    QGst::BufferPtr buffer = pullBuffer();

    // get the image attributes
    QGst::CapsPtr caps = buffer->caps();
    const QGst::StructurePtr structure = caps->internalStructure(0);

    int width, height;
    width = structure.data()->value("width").get<int>();
    height = structure.data()->value("height").get<int>();

    //    qDebug() << Q_FUNC_INFO << "Sample caps:" << structure.data()->toString();

    // create the image from the buffer data
    cv::Mat image(height, width, CV_8UC3, const_cast<uchar*>(buffer->data()));

    // copy the image to be placed in the queue
    cv::Mat* frameImage = new cv::Mat(height, width, CV_8UC3);
    image.copyTo(*frameImage);

    // first check if there is allows place in the queue
    // otherwise remove the tail element
    if (_outputQueue.data()->isOutgrown()) { // TODO : to move this logics to the TimestampedFrameQueue
                                             // class and add a flag to the  constructor
        _outputQueue.data()->dropTail();
    }

    // and push it to the queue
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    TimestampedFrame frame(frameImage, ms);
    _outputQueue.data()->enqueue(frame);

    return QGst::FlowOk;
}
