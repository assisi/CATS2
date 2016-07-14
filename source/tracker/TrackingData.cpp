#include "TrackingData.hpp"

#include "TrackingFactory.hpp"
#include "settings/TrackingSettings.hpp"

#include <TimestampedFrame.hpp>

#include <QtCore/QThread>

/*!
 * Constructor.
 */
TrackingData::TrackingData(SetupType setupType,
                           CoordinatesConversionPtr coordinatesConversion,
                           TimestampedFrameQueuePtr inputQueue,
                           TimestampedFrameQueuePtr debugQueue,
                           QObject *parent) :
    QObject(parent),
    _coordinatesConversion(coordinatesConversion)
{
    // create the tracking routine
    _trackingRoutine = TrackerFactory::createTrackingRoutine(setupType, inputQueue, debugQueue);

    if (!_trackingRoutine.isNull()) {

        // launch the tracking routine in a separated thread
        QThread* thread = new QThread;
        _trackingRoutine.data()->moveToThread(thread);

        connect(thread, &QThread::started, _trackingRoutine.data(), &TrackingRoutine::process);
        connect(_trackingRoutine.data(), &TrackingRoutine::finished, thread, &QThread::quit);
        connect(_trackingRoutine.data(), &TrackingRoutine::finished, [=]() { _trackingRoutine.clear(); });
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        connect(_trackingRoutine.data(), &TrackingRoutine::trackedAgents, this, &TrackingData::onTrackedAgents);

        thread->start();
    }
}

/*!
 * Destructor.
 */
TrackingData::~TrackingData()
{
    if (!_trackingRoutine.isNull())
        _trackingRoutine.data()->stop();
}

/*!
 * Gets the agents from the tracking routine, converts their position in the  world coordinates
 * and sends them further.
 */
void TrackingData::onTrackedAgents(QList<AgentDataImage> agents)
{
    // TODO: add code here
}

