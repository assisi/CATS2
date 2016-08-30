#ifndef CATS2_TRACKING_ROUTINE_WIDGET_HPP
#define CATS2_TRACKING_ROUTINE_WIDGET_HPP

#include "TrackerPointerTypes.hpp"

#include <gui/ViewerWindow.hpp>

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
    explicit TrackingRoutineWidget(TrackingDataPtr trackingData, QWidget *parent = nullptr);
    //! Destructor.
    virtual ~TrackingRoutineWidget() final;

private slots:
    //! Shows/hides the debug viewer window.
    void onShowDebugViewer(bool show);

private:
    //! The data object for the tracking routine.
    TrackingDataPtr m_data;

    //! The debug viewer window.
    ViewerWindow* m_viewerWindow;

    //! The gui form.
    Ui::TrackingRoutineWidget *m_ui;
};

#endif // CATS2_TRACKING_ROUTINE_WIDGET_HPP
