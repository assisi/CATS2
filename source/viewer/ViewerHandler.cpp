#include "ViewerHandler.hpp"
#include "ViewerData.hpp"

#include <QtGui/QImage>

/*!
* Constructor.
*/
ViewerHandler::ViewerHandler(TimestampedFrameQueuePtr inputQueue, QWidget& parent) :
    QObject(&parent),
    _data(QSharedPointer<ViewerData>(new ViewerData(inputQueue), &QObject::deleteLater)),
    _viewerWidget(new ViewerWidget(_data, &parent))
{
    // some security: when the viewer widget is destroyed, reset the pointer to it
    connect(_viewerWidget, &QObject::destroyed, [=]() { _viewerWidget = nullptr; });
}

/*!
* Destructor.
*/
ViewerHandler::~ViewerHandler()
{

}
