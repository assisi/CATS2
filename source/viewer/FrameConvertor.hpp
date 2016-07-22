#ifndef CATS2_FRAME_CONVERTOR_HPP
#define CATS2_FRAME_CONVERTOR_HPP

#include <TimestampedFrame.hpp>

#include <QtCore/QObject>

/*!
* \brief This class receives frames from a queue in openCV Mat format and converts it to Qt image.
* Runs in a separated thread.
*/
class FrameConvertor : public QObject
{
    Q_OBJECT
public:
    //! Constructor.
    explicit FrameConvertor(TimestampedFrameQueuePtr inputQueue);
    //! Destructor.
    virtual ~FrameConvertor();

signals:
    //! Sends out the new frame received.
    void newFrame(QSharedPointer<QImage> frame);
    //! Notifies that the convertor is stopped.
    void finished();

public slots:
    //! Starts the convertor.
    void process();
    //! Stops the convertor.
    void stop();

private:
    //! Converts from openCV Mat to QImage.
    QSharedPointer<QImage> cvMatToQImage(const QSharedPointer<cv::Mat>& imageCv);

private:
    //! The queue containing frames to convert.
    TimestampedFrameQueuePtr m_inputQueue;
    //! The flag that defines if the convertor is to be stopped.
    std::atomic_bool m_stopped;
};

#endif // CATS2_FRAME_CONVERTOR_HPP
