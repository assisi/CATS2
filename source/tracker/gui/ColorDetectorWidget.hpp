#ifndef CATS2_COLOR_DETECTOR_WIDGET_HPP
#define CATS2_COLOR_DETECTOR_WIDGET_HPP

#include "TrackerPointerTypes.hpp"

#include <QtWidgets/QWidget>

namespace Ui {
class ColorDetectorWidget;
}

class ColorDetectorWidget : public QWidget
{
    Q_OBJECT

public:
    //! Constructor.
    explicit ColorDetectorWidget(TrackingRoutinePtr routine, QWidget *parent = nullptr);
    //! Destructor.
    virtual ~ColorDetectorWidget() final;

private:
    //! The gui form.
    Ui::ColorDetectorWidget *m_ui;
    //! The tracking routine.
    TrackingRoutinePtr m_routine;
};

#endif // CATS2_COLOR_DETECTOR_WIDGET_HPP
