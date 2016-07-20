#include "ViewerHandler.hpp"

#include <QtGui/QImage>

/*!
* Constructor.
*/
ViewerHandler::ViewerHandler(TimestampedFrameQueuePtr inputQueue, QWidget& parent) :
    QObject(&parent),
    _data(inputQueue, this),
    _viewerGui(new ViewerWidget(&parent))
{
    qRegisterMetaType<QSharedPointer<QImage>>("QSharedPointer<QImage>");
    connect(&_data, &ViewerData::newFrame, _viewerGui, &ViewerWidget::onNewFrame);

    // some security: when the viewer widget is destroyed, reset the pointer to it
    connect(_viewerGui, &QObject::destroyed, [=]() { _viewerGui = nullptr; });
}

/*!
* Destructor.
*/
ViewerHandler::~ViewerHandler()
{

}
