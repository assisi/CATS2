#ifndef CATS2_FISHBOT_LEDS_TRACKING_WIDGET_HPP
#define CATS2_FISHBOT_LEDS_TRACKING_WIDGET_HPP

#include <QtWidgets/QWidget>

namespace Ui {
class FishBotLedsTrackingWidget;
}

class FishBotLedsTrackingWidget : public QWidget
{
    Q_OBJECT

public:
    //! Constructor.
    explicit FishBotLedsTrackingWidget(QWidget *parent = nullptr);
    //! Destructor.
    virtual ~FishBotLedsTrackingWidget() final;

private:
    //! The gui form.
    Ui::FishBotLedsTrackingWidget *m_ui;
};

#endif // CATS2_FISHBOT_LEDS_TRACKING_WIDGET_HPP
