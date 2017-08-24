#include "RobotsHandler.hpp"

#include "settings/RobotControlSettings.hpp"

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
    // NOTE : this code is commented out because the lambda was called after the
    // destruction of RobotsHandler resulting in a crash
//    // some security: when the widget is destroyed, reset the pointer to it
//    connect(m_widget, &QObject::destroyed, [=]() { m_widget = nullptr; });
}

/*!
* Destructor.
*/
RobotsHandler::~RobotsHandler()
{
    qDebug() << "Destroying the object";
    // if the tracking widget's parent is not set then delete it, otherwise it will stay forever
    // NOTE : at this moment we consider that even if the widget is owned by
    // the main window it's still not destroyed and thus his pointer is not nullptr
    if (/*m_widget &&*/ m_widget->parent() == nullptr)
        m_widget->deleteLater();
}

/*!
 * Returns the pointer to the robots widget.
 */
QWidget* RobotsHandler::widget()
{
    return m_widget;
}

/*!
 * Requests to send the polygon corresponding to the setup.
 */
void RobotsHandler::requestSetupMap()
{
    if (RobotControlSettings::get().setupMap().isValid()) {
        QList<WorldPolygon> polygons = {RobotControlSettings::get().setupMap().polygon()};
        polygons.append(RobotControlSettings::get().setupMap().excludedPolygons());
        AnnotatedPolygons annotatedPolygons;
        annotatedPolygons.polygons = polygons;
        emit notifySetupMap(annotatedPolygons);
    }
}


