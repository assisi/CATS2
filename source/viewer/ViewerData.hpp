#ifndef CATS2_VIEWER_DATA_HPP
#define CATS2_VIEWER_DATA_HPP

#include <TimestampedFrame.hpp>

#include <QtCore/QObject>

class FrameConvertor;

/*!
 * \brief The data class that provides the video frames for the GUI.
 * The data is split on this data class and on the FrameConvertor class that
 * is getting frames from the input queue and converting them to the Qt format
 * and that is run in a separated thread. This split is done in order to encapsulate
 * all the data generation routine in the ViewerData.
 */
class ViewerData : public QObject
{
    Q_OBJECT
public:
    //! Constructor.
    explicit ViewerData(TimestampedFrameQueuePtr inputQueue, QObject *parent = 0);
    //! Destructor.
    virtual ~ViewerData();

signals:
    //! Transfers the new frame further.
    void newFrame(QSharedPointer<QImage> frame);

private:
    //! The frame convertor that receives frame images and converts them to the Qt image format.
    //! Doesn't have a Qt owner as it is managed by another thread.
    QSharedPointer<FrameConvertor> _convertor;
    // TODO : to add the agents here (?)
};

#endif // CATS2_VIEWER_DATA_HPP
