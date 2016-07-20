#ifndef CATS2_VIDEO_GRABBER_HPP
#define CATS2_VIDEO_GRABBER_HPP

#include "StreamReceiver.hpp"

#include <QtCore/QObject>

/*!
* \brief This class manages the video reception from one or several sources. It creates and launches stream receivers in separated threads.
 * NOTE : handler classes should be managed through smart pointers without using the Qt's mechanism
 * of ownership; thus we set the parent to nullptr in the constructor.
*/
class VideoGrabber : public QObject
{
    Q_OBJECT
public:
    //! Constructor.
    explicit VideoGrabber();
    //! Destructor.
    virtual ~VideoGrabber();

public:
    //! Creates and launches a stream receiver in a separated thread.
    void addStreamReceiver(StreamDescriptor parameters, TimestampedFrameQueuePtr outputQueue);
//    void addMultiCameraReceiver();

    //! Stops all the pipelines.
    void stopAll();

private slots:
    void onError(QString errorMessage);

private:
    //! The list of all stream receivers created by the grabber.
    QList<StreamReceiverPtr> _streamReceivers;
};

#endif // CATS2_VIDEO_GRABBER_HPP
