#ifndef CATS2_ROBOTS_HANDLER_HPP
#define ROBOTSHANDLER_H

#include "RobotControlPointerTypes.hpp"

#include <QtCore/QObject>

class RobotsWidget;

/*!
* \brief This class manages the robots control process. It binds together the data and GUI.
 * NOTE : handler classes should be managed through smart pointers without using the Qt's mechanism
 * of ownership; thus we set the parent to nullptr in the constructor.
*/
class RobotsHandler : public QObject
{
    Q_OBJECT

public:
    //! Constructor.
    explicit RobotsHandler();
    //! Destructor.
    virtual ~RobotsHandler() final;

public:
    //! Returns the pointer to the robots widget.
    QWidget* widget();

private:
    //! The control loop class.
    ControlLoopPtr m_contolLoop;
    //! The GUI class.
    RobotsWidget* m_widget;
};

#endif // ROBOTSHANDLER_H
