#ifndef CATS2_STREAM_RECEIVER_H
#define CATS2_STREAM_RECEIVER_H

#include <QMap>
#include <QtCore/QObject>
#include <QGst/Pipeline>
#include "QueueingApplicationSink.hpp"

/*!
 * \brief The type of the incoming video stream
 */
enum class StreamType
{
    VIDEO_4_LINUX,
    UNDEFINED
    // TODO : to be extenced
};

/*!
 * Stores the input stream type and parameters.
 */
class StreamDescriptor
{
public:
    //! Constructor.
    StreamDescriptor(StreamType streamType = StreamType::UNDEFINED, QString parameters = ""):
        _streamType(streamType),
        _parameters(parameters)
    {}

    //! Convenience constructor, takes the stream type as as string.
    StreamDescriptor(QString streamType, QString parameters) :
        _streamType(StreamType::UNDEFINED)
    {
        if (_streamTypeByName.contains(streamType)){
            _streamType = _streamTypeByName[streamType];
            _parameters = parameters;
        }
    }

    //! Getter for the stream type.
    StreamType streamType() { return _streamType; }
    //! Getter for the parameters.
    QString parameters() { return _parameters; }
    //! Checks that the stream descriptor is valid.
    bool isValid() { return (_streamType != StreamType::UNDEFINED); }

    //! Checks that the provided string is a valid stream type.
    static bool isValidStreamType(QString streamType) { return _streamTypeByName.contains(streamType); }

private:
    //! The type of the incoming video stream.
    StreamType _streamType;
    //! Additional parameters of the stream.
    QString _parameters;
    //! The map to translate the string stream type to the corresponding enum.
    static const QMap<QString, StreamType> _streamTypeByName;
};

/*!
 * The alias for the stream parameters list.
 */
using StreamDescriptorList = QList<StreamDescriptor>;

/*!
* \brief This class manages the video stream reception from a specific source. 
*/
class StreamReceiver : public QObject
{
    Q_OBJECT
public:
    //! Constructor for a typified input stream.
    explicit StreamReceiver(StreamDescriptor parameters, TimestampedFrameQueuePtr outputQueue);

    //! Destructor.
    ~StreamReceiver();

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

