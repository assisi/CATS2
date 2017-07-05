#ifndef CATS2_FISHBOT_LEDS_TRACKING_WIDGET_HPP
#define CATS2_FISHBOT_LEDS_TRACKING_WIDGET_HPP

#include "TrackerPointerTypes.hpp"

#include <QtWidgets/QWidget>

namespace Ui {
class FishBotLedsTrackingWidget;
}

class FishBotLedsTrackingWidget : public QWidget
{
    Q_OBJECT

public:
    //! Constructor.
    explicit FishBotLedsTrackingWidget(TrackingRoutinePtr routine,
                                       QWidget *parent = nullptr);
    //! Destructor.
    virtual ~FishBotLedsTrackingWidget() final;

protected slots:
    //! Triggered when the settings are modified.
    void updateSettings();

private:
    //! The gui form.
    Ui::FishBotLedsTrackingWidget *m_ui;
    //! The tracking routine.
    TrackingRoutinePtr m_routine;
};

#endif // CATS2_FISHBOT_LEDS_TRACKING_WIDGET_HPP
