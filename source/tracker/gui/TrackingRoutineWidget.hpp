#ifndef CATS2_TRACKING_ROUTINE_WIDGET_HPP
#define CATS2_TRACKING_ROUTINE_WIDGET_HPP

#include <QtWidgets/QWidget>

namespace Ui {
class TrackingRoutineWidget;
}

/*!
 * \brief The TrackingRoutineWidget class helps to tune and debug the tracking routine.
 */
class TrackingRoutineWidget : public QWidget
{
    Q_OBJECT
public:
    //! Constructor.
    explicit TrackingRoutineWidget(QWidget *parent = nullptr);
    //! Destructor.
    virtual ~TrackingRoutineWidget() final;

private:
    //! The gui form.
    Ui::TrackingRoutineWidget *m_ui;
};

#endif // CATS2_TRACKING_ROUTINE_WIDGET_HPP
