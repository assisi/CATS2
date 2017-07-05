#include "FishBotLedsTrackingWidget.hpp"
#include "ui_FishBotLedsTrackingWidget.h"

#include "FishbotLedColorWidget.hpp"

#include <settings/FishBotLedsTrackingSettings.hpp>
#include <routines/FishBotLedsTracking.hpp>

#include <QtCore/QDebug>
#include <QtWidgets/QLayout>

/*!
 * Constructor.
 */
FishBotLedsTrackingWidget::FishBotLedsTrackingWidget(TrackingRoutinePtr routine,
                                                     QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::FishBotLedsTrackingWidget),
    m_routine(routine)
{
    m_ui->setupUi(this);

    // fill the gui from the routine's settings
    FishBotLedsTracking* tracker = dynamic_cast<FishBotLedsTracking*>(m_routine.data());
    if (tracker) {
        const FishBotLedsTrackingSettingsData& settings = tracker->settings();
        // add a layout
        QVBoxLayout* layout = new QVBoxLayout(m_ui->agentsWidget);
        layout->setMargin(0);
        // make a widget for every robot
        for (int agentIndex = 0; agentIndex < settings.numberOfAgents(); agentIndex++) {
            FishBotLedsTrackingSettingsData::FishBotDescription description =
                    settings.robotDescription(agentIndex);
            FishbotLedColorWidget* widget = new FishbotLedColorWidget(agentIndex,
                                                                      description,
                                                                      this);
            // and place a agent settings widget on this layout
            m_ui->agentsWidget->layout()->addWidget(widget);

            connect(widget, &FishbotLedColorWidget::notifyDescriptionUpdated,
                    [=](int index,
                        FishBotLedsTrackingSettingsData::FishBotDescription description)
                    {
                        FishBotLedsTracking* tracker = dynamic_cast<FishBotLedsTracking*>(m_routine.data());
                        if (tracker) {
                            FishBotLedsTrackingSettingsData settings = tracker->settings();
                            settings.setRobotDescription(index, description);
                            tracker->setSettings(settings);
                        }
                    });
        }
    } else {
        qDebug() << "The tracking routine is ill-defined";
    }
}

/*!
 * Destructor.
 */
FishBotLedsTrackingWidget::~FishBotLedsTrackingWidget()
{
    qDebug() << "Destroying the object";
    delete m_ui;
}

/*!
 * Triggered when the settings are modified.
 */
void FishBotLedsTrackingWidget::updateSettings()
{
    // TODO : to implement
}
