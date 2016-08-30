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
QueueingApplicationSink::QueueingApplicationSink(TimestampedFrameQueuePtr outputQueue, QSize targetFrameSize):
     QGst::Utils::ApplicationSink(),
    m_outputQueue(outputQueue),
    m_convertFrames(targetFrameSize.isValid()),
    m_targetFrameSize(targetFrameSize.width(), targetFrameSize.height())
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

            //    qDebug() << Q_FUNC_INFO << "Sample caps:" << structure->toString();

            // create the image from the buffer data (it makes a pointer to the data in buffer)
            cv::Mat image(height, width, CV_8UC3, const_cast<uchar*>(buffer->data()));

            // copy / resize the image to be placed in the queue (to have data in the image out of the buffer)
            cv::Mat frameImage(height, width, CV_8UC3);
            // resize the image if necessary
            if (m_convertFrames) {
                cv::resize(image, frameImage, m_targetFrameSize, 0, 0, cv::INTER_AREA);
            } else {
                image.copyTo(frameImage);
            }
//            buffer.clear();
            // and push it to the queue
            std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            TimestampedFrame frame(frameImage, ms);
            m_outputQueue->enqueue(frame);
        }
    }

    return QGst::FlowOk;
}
