#include "ViewerHandler.hpp"
#include "ViewerData.hpp"

#include <QtGui/QImage>

/*!
* Constructor.
*/
ViewerHandler::ViewerHandler(TimestampedFrameQueuePtr inputQueue, QWidget& parent) :
    QObject(&parent),
    _viewerGui(new ViewerWidget(&parent))
    _data(QSharedPointer<ViewerData>(new ViewerData(inputQueue), &QObject::deleteLater)),
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
