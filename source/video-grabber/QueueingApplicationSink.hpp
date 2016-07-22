#ifndef CATS2_QUEUEING_APPLICATION_SINK_HPP
#define CATS2_QUEUEING_APPLICATION_SINK_HPP

#include <CommonTypes.hpp>

#include <QGst/Utils/ApplicationSink>
#include <QGst/Structure>

#include <opencv2/core/core.hpp>

#include <QtCore/QObject>

/*!
 * \brief The application sink that puts the recived frames to the queue.
 * Inspired by thsi QtGStreamer example:
 * https://gstreamer.freedesktop.org/data/doc/gstreamer/head/qt-gstreamer/html/examples_2appsink-src_2main_8cpp-example.html
 */
class QueueingApplicationSink : public QGst::Utils::ApplicationSink
{
public:
    //! Constructor. Gets a queue to put the frames in.
    explicit QueueingApplicationSink(TimestampedFrameQueuePtr outputQueue);

protected:
    //! Called when a new sample arrives
    virtual QGst::FlowReturn newBuffer() override;

private:
    //! The queue to put incoming frames.
    TimestampedFrameQueuePtr m_outputQueue;
};


#endif // CATS2_QUEUEING_APPLICATION_SINK_HPP
