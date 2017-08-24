#include "ViewerHandler.hpp"

#include "ViewerData.hpp"
#include "gui/ViewerWidget.hpp"
#include "settings/ViewerSettings.hpp"

#include <QtGui/QImage>
#include <QtCore/QDebug>

/*!
* Constructor.
*/
ViewerHandler::ViewerHandler(SetupType::Enum setupType,
                             TimestampedFrameQueuePtr inputQueue,
                             CoordinatesConversionPtr coordinatesConversion) :
    QObject(nullptr),
    m_data(new ViewerData(inputQueue, coordinatesConversion)),
    m_widget(new ViewerWidget(m_data,
                              ViewerSettings::get().frameSize(setupType),
                              nullptr)) // on creation the widget's parent is
                                        // not set, it is treated in the destructor
{
    // NOTE : this code is commented out because the lambda was called after the
    // destruction of ViewerHandler resulting in a crash
//    // some security: when the viewer widget is destroyed, reset the pointer to it
//    connect(m_widget, &QObject::destroyed, [=]() { m_widget = nullptr; });
}

/*!
* Destructor.
*/
ViewerHandler::~ViewerHandler()
{
    qDebug() << "Destroying the object";
    // if the viewer widget's parent is not set then delete it, otherwise it will stay forever
    // NOTE : at this moment we consider that even if the widget is owned by
    // the main window it's still not destroyed and thus his pointer is not nullptr
    if (/*m_widget && */m_widget->parent() == nullptr)
        m_widget->deleteLater();
}
