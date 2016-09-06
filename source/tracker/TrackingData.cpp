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
    qDebug() << Q_FUNC_INFO << "Destroying the object";
    if (!m_trackingRoutine.isNull())
        m_trackingRoutine->stop();
}

/*!
 * Gets the agents from the tracking routine, converts their position in the  world coordinates
 * and sends them further.
 */
void TrackingData::onTrackedAgents(QList<AgentDataImage> imageAgents)
{
    if (!m_coordinatesConversion.isNull() && m_coordinatesConversion->isValid()) {
        TimestampedWorldAgentData timestampedAgentsData;
        foreach (AgentDataImage imageAgent, imageAgents) {
            // only agents with valid positions are sent further
            if (imageAgent.state().position().isValid()) {
                PositionMeters worldPosition = m_coordinatesConversion->imageToWorldPosition(imageAgent.state().position());
                OrientationRad worldOrientation = m_coordinatesConversion->image2WorldOrientationRad(imageAgent.state().position(),
                                                                                                     imageAgent.state().orientation());
                AgentDataWorld worldAgent(imageAgent.id(),
                                          imageAgent.type(),
                                          StateWorld(worldPosition, worldOrientation));
                timestampedAgentsData.agentsData.append(worldAgent);
                // all agents with valid positions must share the same timestamp
                timestampedAgentsData.timestamp = imageAgent.timestamp();
            }
        }
        if (!timestampedAgentsData.agentsData.isEmpty())
            emit trackedAgents(m_setupType, timestampedAgentsData);
        else
            qDebug() << Q_FUNC_INFO << "Among received agents there is no one with valid position";
    } else {
        qDebug() << Q_FUNC_INFO << "Unable to convert the image coordinates to the world coordinates";
    }
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

/*!
 * Returns a pointer to the tracking routine's debug queue.
 */
TimestampedFrameQueuePtr TrackingData::debugQueue() const
{
    if (!m_trackingRoutine.isNull())
        return m_trackingRoutine->debugQueue();
    else
        return TimestampedFrameQueuePtr();
}
