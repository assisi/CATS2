#include "TrackingHandler.hpp"

#include "TrackingData.hpp"
#include "gui/TrackingRoutineWidget.hpp"

/*!
 * Constructor.
 */
TrackingHandler::TrackingHandler(SetupType setupType,
                                 CoordinatesConversionPtr coordinatesConversion,
                                 TimestampedFrameQueuePtr inputQueue,
                                 QWidget* parentWidget) :
    QObject(nullptr),
    m_data(QSharedPointer<TrackingData>(new TrackingData(setupType,
                                                              coordinatesConversion,
                                                              inputQueue,
                                                              TimestampedFrameQueuePtr()), &QObject::deleteLater)), // FIXME : provide the correct debug queue
    m_widget(new TrackingRoutineWidget(parentWidget))
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

