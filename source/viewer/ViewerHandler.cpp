#include "ViewerHandler.hpp"

#include <QtGui/QImage>

/*!
* Constructor.
*/
ViewerHandler::ViewerHandler(TimestampedFrameQueuePtr inputQueue, ViewerWidget* viewerGui, QObject *parent) :
    QObject(parent),
    _data(inputQueue, this),
    _viewerGui(viewerGui)
{
    qRegisterMetaType<QSharedPointer<QImage>>("QSharedPointer<QImage>");
    connect(&_data, &ViewerData::newFrame, viewerGui, &ViewerWidget::onNewFrame);
}

/*!
* Destructor.
*/
ViewerHandler::~ViewerHandler()
{

}
