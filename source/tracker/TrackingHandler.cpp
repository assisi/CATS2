#include "TrackingHandler.hpp"

#include "TrackingData.hpp"
#include "gui/TrackingRoutineWidget.hpp"

/*!
 * Constructor.
 */
TrackingHandler::TrackingHandler(SetupType::Enum setupType,
                                 CoordinatesConversionPtr coordinatesConversion,
                                 TimestampedFrameQueuePtr inputQueue) :
    QObject(nullptr),
    m_debugQueue(new TimestampedFrameQueue(100)),
    m_data(TrackingDataPtr(new TrackingData(setupType,
                                            coordinatesConversion,
                                            inputQueue,
                                            m_debugQueue),
                                            &QObject::deleteLater)),
    m_widget(new TrackingRoutineWidget(nullptr)) // on creation the widget's parent is not set, it is treated in the destructor
{
    // some security: when the tracking widget is destroyed, reset the pointer to it
    connect(m_widget, &QObject::destroyed, [=]() { m_widget = nullptr; });
}

/*!
* Destructor.
*/
TrackingHandler::~TrackingHandler()
{
    // if the tracking widget's parent is not set then delete it, otherwise it will stay forever
    if (m_widget->parent() == nullptr)
        delete m_widget;
}

