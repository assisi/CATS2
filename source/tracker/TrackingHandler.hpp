#ifndef CATS2_TRACKING_HANDLER_HPP
#define CATS2_TRACKING_HANDLER_HPP

#include <SetupType.hpp>
#include <CoordinatesConversion.hpp>
#include <TimestampedFrame.hpp>

#include <QObject>

class TrackingData;
class TrackingRoutineWidget;

/*!
* \brief This class manages the tracking process. It binds together the data and GUI.
 * NOTE : handler classes should be managed through smart pointers without using the Qt's mechanism
 * of ownership; thus we set the parent to nullptr in the constructor.
*/
class TrackingHandler : public QObject
{
    Q_OBJECT
public:
    //! Constructor. It receives a qwidget to pass as a parent to the tracking routine widget.
    explicit TrackingHandler(SetupType setupType, CoordinatesConversionPtr coordinatesConversion,
                             TimestampedFrameQueuePtr inputQueue, QWidget* parentWidget);
    //! Destructor.
    virtual ~TrackingHandler();

public:
    //! Returns the pointer to the tracking widget.
    TrackingRoutineWidget* widget() { return _widget; }

private:
    //! The data class.
    QSharedPointer<TrackingData> _data;
    //! The GUI class.
    TrackingRoutineWidget* _widget;
};

#endif // CATS2_TRACKING_HANDLER_HPP
