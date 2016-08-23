#ifndef CATS2_TRACKING_DATA_HPP
#define CATS2_TRACKING_DATA_HPP

#include "routines/TrackingRoutine.hpp"
#include "routines/TrackingRoutineType.hpp"
#include "TrackerPointerTypes.hpp"

#include <CommonPointerTypes.hpp>
#include <SetupType.hpp>

#include <QtCore/QObject>

class AgentDataWorld;
class AgentDataImage;

/*!
 * \brief The data class that launches the tracking routine.
 * Which tracking method to use and its parameters is defined by the
 * TrackingSettings settings singleton.
 * NOTE : data classes should be managed through smart pointers without using the Qt's mechanism
 * of ownership; thus we set the parent to nullptr in the constructor.
 */
class TrackingData : public QObject
{
    Q_OBJECT
public:
    //! Constructor.
    explicit TrackingData(SetupType::Enum setupType,
                          CoordinatesConversionPtr coordinatesConversion,
                          TimestampedFrameQueuePtr inputQueue,
                          TimestampedFrameQueuePtr debugQueue);
    //! Destructor.
    virtual ~TrackingData();

public:
    //! Returns a pointer to the tracking routine's debug queue.
    TimestampedFrameQueuePtr debugQueue() const;
    //! The type of setup for which this tracking is used.
    SetupType::Enum setupType() const { return m_setupType; }
    //! The type of the tracking routine.
    TrackingRoutineType::Enum trackingType() const;
    //! Getter for the pointer to the coordinates conversion.
    CoordinatesConversionPtr coordinatesConversion() const { return m_coordinatesConversion; }

signals:
    //! Sends out the tracked agents in world coordinates. Also the setup type is send to "sign" the signal.
    void trackedAgents(SetupType::Enum setupType, QList<AgentDataWorld> worldAgents);
    //! Request to start/stop enqueueing the debug images to the debug queue.
    void sendDebugImages(bool send);

private slots:
    //! Gets the agents from the tracking routine, converts their position in the
    //! world coordinates and sends them further.
    void onTrackedAgents(QList<AgentDataImage> agents);

private:
    //! The type of setup, for instance, main camera or the camera below.
    //! It's stored here to be provided to the gui on request.
    SetupType::Enum m_setupType;

    //! The tracking routine that tracks agents on the scene.
    //! Doesn't have a Qt owner as it is managed by another thread.
    TrackingRoutinePtr m_trackingRoutine;

    //! The coordinates conversion to get world state of the agents.
    CoordinatesConversionPtr m_coordinatesConversion;
};

#endif // CATS2_TRACKING_DATA_HPP
