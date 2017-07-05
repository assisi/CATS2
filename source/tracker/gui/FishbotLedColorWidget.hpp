#ifndef CATS2_FISHBOT_LED_COLOR_HPP
#define CATS2_FISHBOT_LED_COLOR_HPP

#include <settings/FishBotLedsTrackingSettings.hpp>

#include <QtWidgets/QWidget>

namespace Ui {
class FishbotLedColorWidget;
}

class FishbotLedColorWidget : public QWidget
{
    Q_OBJECT

public:
    //! Constructor.
    explicit FishbotLedColorWidget(int index,
                                   FishBotLedsTrackingSettingsData::FishBotDescription description,
                                   QWidget *parent = 0);
    //! Destructor.
    virtual ~FishbotLedColorWidget();

signals:
    //! Notifies on the description update.
    void notifyDescriptionUpdated(int index,
                                  FishBotLedsTrackingSettingsData::FishBotDescription description);

protected slots:
    //! Triggered when the settings are modified.
    void updateSettings();

private:
    //! The gui form.
    Ui::FishbotLedColorWidget *m_ui;
    //! The index of the this agent widget.
    int m_index;
    //! The agent's id.
    QString m_id;
    //! The robot LEDs' color.
    QColor m_ledColor;
    //! The color detection threshold.
    int m_colorThreshold;
};

#endif // CATS2_FISHBOT_LED_COLOR_HPP
