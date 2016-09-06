#ifndef CATS2_TRACKING_HANDLER_HPP
#define CATS2_TRACKING_HANDLER_HPP

#include "TrackerPointerTypes.hpp"

#include <CommonPointerTypes.hpp>
#include <SetupType.hpp>
#include <CoordinatesConversion.hpp>
#include <TimestampedFrame.hpp>

#include <QtCore/QObject>

class TrackingRoutineWidget;
enum class AgentType;

/*!
* \brief This class manages the tracking process. It binds together the data and GUI.
 * NOTE : handler classes should be managed through smart pointers without using the Qt's mechanism
 * of ownership; thus we set the parent to nullptr in the constructor.
*/
class TrackingHandler : public QObject
{
    Q_OBJECT
public:
    //! Constructor.
    explicit TrackingHandler(SetupType::Enum setupType,
                             CoordinatesConversionPtr coordinatesConversion,
                             TimestampedFrameQueuePtr inputQueue);
    //! Destructor.
    virtual ~TrackingHandler() final;

public:
    //! Returns the pointer to the tracking widget.
    TrackingRoutineWidget* widget() { return m_widget; }
    //! Returns the pointer to the tracking data.
    TrackingDataPtr data() const { return m_data; } // NOTE : this is done for the
                                                    // sake of time and is not correct
                                                    // from the corrent incapsulation
                                                    // point of view

private:
    //! Debug queue for the tracker.
    TimestampedFrameQueuePtr m_debugQueue;
    //! The data class.
    TrackingDataPtr m_data;
    //! The GUI class.
    TrackingRoutineWidget* m_widget;
};

#endif // CATS2_TRACKING_HANDLER_HPP
