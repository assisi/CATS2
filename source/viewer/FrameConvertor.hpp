#ifndef CATS2_FRAME_CONVERTOR_HPP
#define CATS2_FRAME_CONVERTOR_HPP

#include <TimestampedFrame.hpp>

#include <QtCore/QObject>

/*!
* \brief This class receives frames from a queue in openCV Mat format and converts it to Qt image.
*/
class FrameConvertor : public QObject
{
    Q_OBJECT
public:
    //! Constructor for a typified input stream.
    explicit FrameConvertor(TimestampedFrameQueuePtr inputQueue);
    //! Destructor.
    virtual ~FrameConvertor();

signals:
    //! Sends out the new frame received.
    void newFrame(QSharedPointer<QImage> frame);
    //! Notifies that the convertor is stopped.
    void finished();

public slots:
    //! Starts the receiver.
    void process();
    //! Stops the receiver.
    void stop();

private:
    //! Converts from openCV Mat to QImage.
    QSharedPointer<QImage> cvMatToQImage(const QSharedPointer<cv::Mat>& imageCv);

private:
    //! Dequeueing time out.
    static const int TimeOutMs;  // [ms]

    //! The queue containing frames to conver.
    TimestampedFrameQueuePtr _inputQueue;
    //! The flag that defines if the convertor is to be stopped.
    std::atomic_bool _stopped;
};

#endif // CATS2_FRAME_CONVERTOR_HPP
