#include "QueueingApplicationSink.hpp"

#include <TimestampedFrame.hpp>

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
QueueingApplicationSink::QueueingApplicationSink(TimestampedFrameQueuePtr outputQueue, QSize expectedFrameSize):
     QGst::Utils::ApplicationSink(),
    m_outputQueue(outputQueue),
    m_expectedFrameSize(expectedFrameSize.width(), expectedFrameSize.height())
{
}

/*!
 * Called when a new sample arrives.
 */
QGst::FlowReturn QueueingApplicationSink::newBuffer()
{
    QGst::BufferPtr buffer = pullBuffer();

    if (!buffer.isNull()) {
        // get the image attributes
        QGst::CapsPtr caps = buffer->caps();
        const QGst::StructurePtr structure = caps->internalStructure(0);

        if (!structure.isNull()) {
            int width, height;
            width = structure->value("width").get<int>();
            height = structure->value("height").get<int>();

//            qDebug() << "Sample caps:" << structure->toString();

            // check the size of the incoming frames, if it's not as expected
            // then return an error to the pipeline
            if (width != m_expectedFrameSize.width || height != m_expectedFrameSize.height) {
                qDebug() << QString("Video resolution %1x%2 is not as expected %3x%4")
                            .arg(width)
                            .arg(height)
                            .arg(m_expectedFrameSize.width)
                            .arg(m_expectedFrameSize.height);
                return QGst::FlowNotSupported;
            }

            // create the image from the buffer data (it makes a pointer to the data in buffer)
            cv::Mat image(height, width, CV_8UC3, const_cast<uchar*>(buffer->data()));

            // copy the image to be placed in the queue (to have data in the image out of the buffer)
            cv::Mat frameImage(height, width, CV_8UC3);
            image.copyTo(frameImage);

//            buffer.clear();
            // and push it to the queue
            std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            TimestampedFrame frame(frameImage, ms);
            m_outputQueue->enqueue(frame);
        }
    }

    return QGst::FlowOk;
}
