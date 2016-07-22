#ifndef CATS2_TRACKING_DATA_HPP
#define CATS2_TRACKING_DATA_HPP

#include "routines/TrackingRoutine.hpp"

#include <CoordinatesConversion.hpp>
#include <CommonPointerTypes.hpp>
#include <SetupType.hpp>

#include <QObject>

class TrackingRoutine;
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

signals:
    //! Sends out the tracked agents.
    void trackedAgents(QList<AgentDataWorld> agents);

private slots:
    //! Gets the agents from the tracking routine, converts their position in the
    //! world coordinates and sends them further.
    void onTrackedAgents(QList<AgentDataImage> agents);

private:
    //! The tracking routine that tracks agents on the scene.
    //! Doesn't have a Qt owner as it is managed by another thread.
    QSharedPointer<TrackingRoutine> m_trackingRoutine;

    //! The coordinates conversion to get world state of the agents.
    CoordinatesConversionPtr m_coordinatesConversion;
};

#endif // CATS2_TRACKING_DATA_HPP
