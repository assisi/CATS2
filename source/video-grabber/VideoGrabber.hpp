#ifndef CATS2_VIDEO_GRABBER_HPP
#define CATS2_VIDEO_GRABBER_HPP

#include <QtCore/QObject>

#include "StreamReceiver.hpp"

/*!
* \brief This class manages the video reception from one or several sources. It creates and launches stream receivers in separated threads.
*
*/
class VideoGrabber : public QObject
{
    Q_OBJECT
public:
    //! Constructor.
    explicit VideoGrabber();
    //! Destructor.
    virtual ~VideoGrabber();

    //! Creates and launches a stream receiver in a separated thread.
    void addStreamReceiver(StreamDescriptor parameters, TimestampedFrameQueuePtr outputQueue);
//    void addMultiCameraReceiver();

private slots:
    void onError(QString errorMessage);

private:
    //! The list of all stream receivers created by the grabber.
    QList<StreamReceiverPtr> _streamReceivers;
};

#endif // CATS2_VIDEO_GRABBER_HPP
