#ifndef CATS2_STREAM_RECEIVER_H
#define CATS2_STREAM_RECEIVER_H

#include "QueueingApplicationSink.hpp"

#include <settings/CommandLineParameters.hpp>

#include <QGst/Pipeline>

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QSize>

/*!
* \brief This class manages the video stream reception from a specific source.
* Runs in a separated thread.
*/
class StreamReceiver : public QObject
{
    Q_OBJECT
public:
    //! Constructor for a typified input stream.
    explicit StreamReceiver(StreamDescriptor parameters, QSize expectedFrameSize, TimestampedFrameQueuePtr outputQueue);

    //! Destructor.
    virtual ~StreamReceiver();

public slots:
    //! Starts the receiver.
    void process();
    //! Stops the receiver.
    void stop();

private slots:
    //! Called when an error is detected in the input pipelile.
    void onMessage(const QGst::MessagePtr& message);

signals:
    //! Emitted when an error is encountered.
    void error(QString errorMessage);

private:
    //! Tries to restart the video stream.
    void restart();

private:
    //! The pipeline description to set the gstreamer.
    QString m_pipelineDescription;
    //! The acquisition pipeline.
    QGst::PipelinePtr m_pipeline;
    //! The sink that recieives frames.
    QueueingApplicationSink m_sink;

    //! The flag that specifies if the video stream should be restarted when
    //! an "end-of-stream" message is received.
    bool m_restartOnEos;

    //! The expected image size.
    QSize m_expectedFrameSize;
};

#endif // CATS2_STREAM_RECEIVER_H

