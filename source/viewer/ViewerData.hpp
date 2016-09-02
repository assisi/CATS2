#ifndef CATS2_VIEWER_DATA_HPP
#define CATS2_VIEWER_DATA_HPP

#include "ViewerPointerTypes.hpp"

#include <CommonPointerTypes.hpp>

#include <QtCore/QObject>
#include <QtGui/QPixmap>

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
    //! Constructor. Receives the coordinates conversion for the setup to which corresponds this viewer.
    explicit ViewerData(TimestampedFrameQueuePtr inputQueue,
                        CoordinatesConversionPtr coordinatesConversion = CoordinatesConversionPtr());
    //! Destructor.
    virtual ~ViewerData();

public:
    //! Returns the coordinates transformation.
    CoordinatesConversionPtr coordinatesConversion() { return m_coordinatesConversion; }

signals:
    //! Transfers the new frame further (with the corresponding frame rate.
    void newFrame(QSharedPointer<QPixmap> frame, int fps);

private:
    //! The frame convertor that receives frame images and converts them to the Qt image format.
    //! Doesn't have a Qt owner as it is managed by another thread.
    FrameConvertorPtr m_frameConvertor;
    //! The coordinates transformation to convert positions between meters and camera frame's pixels.
    CoordinatesConversionPtr m_coordinatesConversion;
    // TODO : to add the agents here (?)
};

#endif // CATS2_VIEWER_DATA_HPP
