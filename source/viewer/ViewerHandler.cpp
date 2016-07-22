#include "ViewerHandler.hpp"

#include "ViewerData.hpp"
#include "ViewerWidget.hpp"

#include <QtGui/QImage>

/*!
* Constructor.
*/
ViewerHandler::ViewerHandler(TimestampedFrameQueuePtr inputQueue, QWidget* parentWidget) :
    QObject(nullptr),
    m_data(QSharedPointer<ViewerData>(new ViewerData(inputQueue), &QObject::deleteLater)),
    m_widget(new ViewerWidget(m_data, parentWidget))
{
    // some security: when the viewer widget is destroyed, reset the pointer to it
    connect(m_widget, &QObject::destroyed, [=]() { m_widget = nullptr; });
}

/*!
* Destructor.
*/
ViewerHandler::~ViewerHandler()
{
    // if the viewer widget's parent is not set then delete it, otherwise it will stay forever
    if (m_widget->parent() == nullptr)
        delete m_widget;
}
