#include "TrackingHandler.hpp"

#include "TrackingData.hpp"
#include "gui/TrackingRoutineWidget.hpp"

#include <CoordinatesConversion.hpp>
#include <TimestampedFrame.hpp>

/*!
 * Constructor.
 */
TrackingHandler::TrackingHandler(SetupType::Enum setupType,
                                 CoordinatesConversionPtr coordinatesConversion,
                                 TimestampedFrameQueuePtr inputQueue) :
    QObject(nullptr),
    m_debugQueue(new TimestampedFrameQueue(100)),
    m_data(new TrackingData(setupType,
                            coordinatesConversion,
                            inputQueue,
                            m_debugQueue)),
    m_widget(new TrackingRoutineWidget(m_data, nullptr)) // on creation the widget's parent is not set, it is treated in the destructor
{
    // NOTE : this code is commented out because the lambda was called after the
    // destruction of TrackingHandler resulting in a crash
//    // some security: when the tracking widget is destroyed, reset the pointer to it
//    connect(m_widget, &QObject::destroyed, [=]() { m_widget = nullptr; });
}

/*!
* Destructor.
*/
TrackingHandler::~TrackingHandler()
{
    qDebug() << "Destroying the object";
    // if the tracking widget's parent is not set then delete it, otherwise it will stay forever
    // NOTE : at this moment we consider that even if the widget is owned by
    // the main window it's still not destroyed and thus his pointer is not nullptr
    if (/*m_widget &&*/ m_widget->parent() == nullptr)
        m_widget->deleteLater();
}
