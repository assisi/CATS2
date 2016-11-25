#include "RobotsHandler.hpp"

#include "gui/RobotsWidget.hpp"
#include "ControlLoop.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
RobotsHandler::RobotsHandler() :
    QObject(nullptr),
    m_contolLoop(new ControlLoop()),
    m_widget(new RobotsWidget(m_contolLoop, nullptr)) // on creation the widget's parent is not set, it is treated in the destructor
{
    // some security: when the widget is destroyed, reset the pointer to it
    connect(m_widget, &QObject::destroyed, [=]() { m_widget = nullptr; });
}

/*!
* Destructor.
*/
RobotsHandler::~RobotsHandler()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
    // if the tracking widget's parent is not set then delete it, otherwise it will stay forever
    if (m_widget && m_widget->parent() == nullptr)
        delete m_widget;
}

/*!
 * Returns the pointer to the robots widget.
 */
QWidget* RobotsHandler::widget()
{
    return m_widget;
}


