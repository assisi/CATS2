#ifndef CATS2_VIEWER_DATA_HPP
#define CATS2_VIEWER_DATA_HPP

#include <CommonPointerTypes.hpp>
#include <ViewerPointerTypes.hpp>

#include <QtCore/QObject>

/*!
 * \brief The data class that provides the video frames for the GUI.
 * The data is split on this data class and on the FrameConvertor class that
 * is getting frames from the input queue and converting them to the Qt format
 * and that is run in a separated thread. This split is done in order to encapsulate
 * all the data generation routine in the ViewerData.
 * NOTE : data classes should be managed through smart pointers without using the Qt's mechanism
 * of ownership; thus we set the parent to nullptr in the constructor.
 */
class ViewerData : public QObject
{
    Q_OBJECT
public:
    //! Constructor.
    explicit ViewerData(TimestampedFrameQueuePtr inputQueue);
    //! Destructor.
    virtual ~ViewerData();

signals:
    //! Transfers the new frame further.
    void newFrame(QSharedPointer<QImage> frame);

private:
    //! The frame convertor that receives frame images and converts them to the Qt image format.
    //! Doesn't have a Qt owner as it is managed by another thread.
    QSharedPointer<FrameConvertor> m_convertor;
    // TODO : to add the agents here (?)
};

#endif // CATS2_VIEWER_DATA_HPP
