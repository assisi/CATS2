#ifndef CATS2_STREAM_RECEIVER_H
#define CATS2_STREAM_RECEIVER_H

#include "QueueingApplicationSink.hpp"

#include <CommandLineParameters.hpp>

#include <QGst/Pipeline>

#include <QtCore/QMap>
#include <QtCore/QObject>

/*!
* \brief This class manages the video stream reception from a specific source.
* Runs in a separated thread.
*/
class StreamReceiver : public QObject
{
    Q_OBJECT
public:
    //! Constructor for a typified input stream.
    explicit StreamReceiver(StreamDescriptor parameters, TimestampedFrameQueuePtr outputQueue);

    //! Destructor.
    virtual ~StreamReceiver();

public slots:
    //! Starts the receiver.
    void process();
    //! Stops the receiver.
    void stop();

private slots:
    //! Called when an error is detected in the input pipelile.
    void onError(const QGst::MessagePtr& message);

signals:
    //! Emitted when an error is encountered.
    void error(QString errorMessage);

private:
    //! The pipeline description to set the gstreamer.
    QString _pipe1ineDescription;
    //! The acquisition pipeline.
    QGst::PipelinePtr _pipeline;
    //! The sink that recieives frames.
    QueueingApplicationSink _sink;
};

/*!
 * The alias for the stream receiver shared pointer.
 */
using StreamReceiverPtr = QSharedPointer<StreamReceiver>;


#endif // CATS2_STREAM_RECEIVER_H

