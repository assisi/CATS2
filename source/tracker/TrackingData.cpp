#include "TrackingData.hpp"

#include "TrackingFactory.hpp"
#include "settings/TrackingSettings.hpp"

#include <AgentData.hpp>
#include <TimestampedFrame.hpp>
#include <CoordinatesConversion.hpp>

#include <QtCore/QThread>

/*!
 * Constructor.
 */
TrackingData::TrackingData(SetupType::Enum setupType,
                           CoordinatesConversionPtr coordinatesConversion,
                           TimestampedFrameQueuePtr inputQueue,
                           TimestampedFrameQueuePtr debugQueue) :
    QObject(nullptr),
    m_setupType(setupType),
    m_coordinatesConversion(coordinatesConversion)
{
    qRegisterMetaType<QList<AgentDataImage>>("QList<AgentDataImage>");

    // create the tracking routine
    m_trackingRoutine = TrackerFactory::createTrackingRoutine(setupType, inputQueue, debugQueue);

    if (!m_trackingRoutine.isNull()) {

        // launch the tracking routine in a separated thread
        QThread* thread = new QThread;
        m_trackingRoutine->moveToThread(thread);

        connect(thread, &QThread::started, m_trackingRoutine.data(), &TrackingRoutine::process);
        connect(m_trackingRoutine.data(), &TrackingRoutine::finished, thread, &QThread::quit);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        connect(m_trackingRoutine.data(), &TrackingRoutine::trackedAgents, this, &TrackingData::onTrackedAgents);
        connect(this, &TrackingData::sendDebugImages,
                m_trackingRoutine.data(), &TrackingRoutine::onSendDebugImages, Qt::DirectConnection);  // NOTE : direct connection is to ensure that the slot is called, but we need to ensure that it is thread safe
        thread->start();
    }
}

/*!
 * Destructor.
 */
TrackingData::~TrackingData()
{
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

/*!
 * The type of the tracking routine.
 */
TrackingRoutineType::Enum TrackingData::trackingType() const
{
    // first get the tracking settings for the current setup
    TrackingRoutineSettingsPtr settings = TrackingSettings::get().trackingRoutineSettings(m_setupType);

    if (!settings.isNull())
        return settings->type();
    else
        return TrackingRoutineType::UNDEFINED;
}
