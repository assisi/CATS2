#include "TrackingSetup.hpp"
#include "TrackingData.hpp"
#include "TrackingDataManager.hpp"
#include "TrackingHandler.hpp"

#include <TimestampedFrame.hpp>
#include <CoordinatesConversion.hpp>
#include <GrabberHandler.hpp>
#include <QueueHub.hpp>
#include <AgentData.hpp>

#include <settings/CalibrationSettings.hpp>

#include <gui/TrackingRoutineWidget.hpp>

/*!
 * Constructor.
 */
TrackingSetup::TrackingSetup(SetupType::Enum setupType, bool needOutputQueue) :
    m_setupType(setupType),
    m_coordinatesConversion(new CoordinatesConversion(CalibrationSettings::get()
                                                      .calibrationFilePath(setupType),
                                                      CalibrationSettings::get()
                                                      .frameSize(setupType))),
    m_grabber(new GrabberHandler(setupType))
{
    // if there is no need to expose camera images in the additional queue then
    // the tracker gets directly the images from the grabber
    if (!needOutputQueue) {
        m_tracking = TrackingHandlerPtr(new TrackingHandler(setupType,
                                                            m_coordinatesConversion,
                                                            m_grabber->inputQueue()));
    } else {
        // otherwise we need to introduce the multiplicator that will take care
        // about the extra queue
        m_queueHub = QueueHubPtr(new QueueHub(m_grabber->inputQueue()));
        m_tracking = TrackingHandlerPtr(new TrackingHandler(setupType,
                                                            m_coordinatesConversion,
                                                            m_queueHub->addOutputQueue()));
    }
}

/*!
 * Destructor.
 */
TrackingSetup::~TrackingSetup()
{
    qDebug() << "Destroying the object";
}


/*!
 * Returns the pointer to the hub's output queue.
 */
TimestampedFrameQueuePtr TrackingSetup::viewerQueue()
{
    if (!m_queueHub.isNull())
        return m_queueHub->addOutputQueue();
    else
        return TimestampedFrameQueuePtr();
}

/*!
 * Returns the pointer to the tracking routine gui.
 */
QWidget* TrackingSetup::trackingWidget()
{
    return m_tracking->widget();
}

/*!
 * Registers the data source in the data manager and makes the necessary connections.
 */
void TrackingSetup::connectToDataManager(TrackingDataManagerPtr& trackingDataManager)
{
    trackingDataManager->addDataSource(m_setupType, m_tracking->data()->routineCapabilities());
    trackingDataManager->addCoordinatesConversion(m_setupType, m_coordinatesConversion);
    QObject::connect(m_tracking->data().data(), &TrackingData::trackedAgents,
                     trackingDataManager.data(), &TrackingDataManager::onNewData);
}
