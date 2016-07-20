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
    _data(QSharedPointer<TrackingData>(new TrackingData(setupType,
                                                              coordinatesConversion,
                                                              inputQueue,
                                                              TimestampedFrameQueuePtr()), &QObject::deleteLater)), // FIXME : provide the correct debug queue
    _widget(new TrackingRoutineWidget(parentWidget))
{
    // some security: when the tracking widget is destroyed, reset the pointer to it
    connect(_widget, &QObject::destroyed, [=]() { _widget = nullptr; });
}

/*!
* Destructor.
*/
TrackingHandler::~TrackingHandler()
{
    // if the tracking widget's parent is not set then delete it, otherwise it will stay forever
    if (_widget->parent() == nullptr)
        delete _widget;
}

