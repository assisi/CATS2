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
QGst::FlowReturn QueueingApplicationSink::newSample()
{
    QGst::SamplePtr sample = pullSample();
    // get the image attributes
    QGst::CapsPtr caps = sample->caps();
    const QGst::StructurePtr structure = caps->internalStructure(0);

    int width, height;
    width = structure.data()->value("width").get<int>();
    height = structure.data()->value("height").get<int>();

//    qDebug() << "Sample caps:" << structure.data()->toString();

    // this part is inspired by the posts
    // (1) http://stackoverflow.com/questions/35983701/qtgstreamer-how-to-take-a-snapshot-while-playing-live-video-stream
    // (2) https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap
    /*
     * QImage::Format_RGBX8888 corresponds to GST_VIDEO_FORMAT_RGBx,
     * QImage::Format_RGBA8888 corresponds to GST_VIDEO_FORMAT_RGBA,
     * QImage::Format_RGB888   corresponds to GST_VIDEO_FORMAT_RGB,
     * QImage::Format_RGB16    corresponds to GST_VIDEO_FORMAT_RGB16
     */
    GstCaps * capsTo = gst_caps_new_simple("video/x-raw",
                                 "format", G_TYPE_STRING, "RGB",
                                 "width", G_TYPE_INT, width,
                                 "height", G_TYPE_INT, height,
                                 nullptr);

    GError *err = NULL;
    QGst::SamplePtr convertedSample = QGst::SamplePtr::wrap(gst_video_convert_sample(sample, capsTo, GST_SECOND, &err));
    if (convertedSample.isNull()) {
        qWarning() << "Video conversion failed:" << err->message;
    } else {
        QGst::BufferPtr buffer = convertedSample->buffer();
        QGst::MapInfo mapInfo;
        buffer->map(mapInfo, QGst::MapRead);

        // use the constructor to make a header for user-allocated data
        cv::Mat imageRGB( height, width, CV_8UC3, const_cast<uchar*>(mapInfo.data()));

        // convert image to BGR for further processing
        cv::Mat* imageBGR = new cv::Mat(height, width, CV_8UC3);
        cv::cvtColor(imageRGB, *imageBGR, CV_RGB2BGR);

        // first check if there is allows place in the queue
        // otherwise remove the tail element
        if (_outputQueue.data()->isOutgrown()) {
            _outputQueue.data()->dropTail();
        }

        // and push it to the queue
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        TimestampedFrame frame(imageBGR, ms);
        _outputQueue.data()->enqueue(frame);

//        QString path = QString("images/image_%1.jpg").arg(_counter);
//        cv::imwrite(path.toStdString(), imageBGR );
//        _counter++;

        buffer->unmap(mapInfo);
    }

    return QGst::FlowOk;
}
