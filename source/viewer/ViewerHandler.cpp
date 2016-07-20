#include "ViewerHandler.hpp"

#include "ViewerData.hpp"
#include "ViewerWidget.hpp"

#include <QtGui/QImage>

/*!
* Constructor.
*/
ViewerHandler::ViewerHandler(TimestampedFrameQueuePtr inputQueue, QWidget* parentWidget) :
    QObject(nullptr),
    _data(QSharedPointer<ViewerData>(new ViewerData(inputQueue), &QObject::deleteLater)),
    _widget(new ViewerWidget(_data, parentWidget))
{
    // some security: when the viewer widget is destroyed, reset the pointer to it
    connect(_widget, &QObject::destroyed, [=]() { _widget = nullptr; });
}

/*!
* Destructor.
*/
ViewerHandler::~ViewerHandler()
{
    // if the viewer widget's parent is not set then delete it, otherwise it will stay forever
    if (_widget->parent() == nullptr)
        delete _widget;
}
