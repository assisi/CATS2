#include "TrackingData.hpp"

#include "TrackingFactory.hpp"
#include "settings/TrackingSettings.hpp"

#include <AgentData.hpp>
#include <TimestampedFrame.hpp>

#include <QtCore/QThread>

/*!
 * Constructor.
 */
TrackingData::TrackingData(SetupType::Enum setupType,
                           CoordinatesConversionPtr coordinatesConversion,
                           TimestampedFrameQueuePtr inputQueue,
                           TimestampedFrameQueuePtr debugQueue) :
    QObject(nullptr),
    m_coordinatesConversion(coordinatesConversion)
{
    // create the tracking routine
    m_trackingRoutine = TrackerFactory::createTrackingRoutine(setupType, inputQueue, debugQueue);

    if (!m_trackingRoutine.isNull()) {

        // launch the tracking routine in a separated thread
        QThread* thread = new QThread;
        m_trackingRoutine->moveToThread(thread);

        connect(thread, &QThread::started, m_trackingRoutine.data(), &TrackingRoutine::process);
        connect(m_trackingRoutine.data(), &TrackingRoutine::finished, thread, &QThread::quit);
        connect(m_trackingRoutine.data(), &TrackingRoutine::finished, [=]() { m_trackingRoutine.clear(); });
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        connect(m_trackingRoutine.data(), &TrackingRoutine::trackedAgents, this, &TrackingData::onTrackedAgents);

        thread->start();
    }
}

/*!
 * Destructor.
 */
TrackingData::~TrackingData()
{
    if (!m_trackingRoutine.isNull())
        m_trackingRoutine->stop();
}

/*!
 * Gets the agents from the tracking routine, converts their position in the  world coordinates
 * and sends them further.
 */
void TrackingData::onTrackedAgents(QList<AgentDataImage> agents)
{
    // TODO: add code here
}

