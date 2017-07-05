#include "FishbotLedColorWidget.hpp"
#include "ui_FishbotLedColorWidget.h"

#include <QtWidgets/QColorDialog>

/*!
 * Constructor.
 */
FishbotLedColorWidget::FishbotLedColorWidget(int index,
                                             FishBotLedsTrackingSettingsData::FishBotDescription description,
                                             QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::FishbotLedColorWidget),
    m_index(index),
    m_id(description.id)
{
    m_ui->setupUi(this);

    m_ui->tagGroupBox->setTitle(QString("Robot %1").arg(m_id));

    m_ui->tagColorValueLabel->setText(QString("(%1,%2,%3)")
                                      .arg(description.ledColor.red())
                                      .arg(description.ledColor.green())
                                      .arg(description.ledColor.blue()));
    m_ui->thresholdValueLabel->setText(QString::number(description.colorThreshold));
    m_ui->thresholdSlider->setValue(description.colorThreshold);
    m_ledColor = description.ledColor;

    connect(m_ui->thresholdSlider,
            static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged),
            this, &FishbotLedColorWidget::updateSettings);
    connect(m_ui->thresholdSlider,
            static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged),
            [=](int value) {
        m_ui->thresholdValueLabel->setText(QString::number(value));
    });

    connect(m_ui->tagColorButton, &QPushButton::clicked,
            [=]() {
        QColor color = QColorDialog::getColor(m_ledColor, this);
        if (color.isValid()) {
            m_ledColor = color;
            updateSettings();

            m_ui->tagColorValueLabel->setText(QString("(%1,%2,%3)")
                                              .arg(m_ledColor.red())
                                              .arg(m_ledColor.green())
                                              .arg(m_ledColor.blue()));
        }
    });
}

/*!
 * Destructor.
 */
FishbotLedColorWidget::~FishbotLedColorWidget()
{
    qDebug() << "Destroying the object";
    delete m_ui;
}

/*!
 * Triggered when the settings are modified.
 */
void FishbotLedColorWidget::updateSettings()
{
    FishBotLedsTrackingSettingsData::FishBotDescription description;

    description.id = m_id;
    description.colorThreshold = m_ui->thresholdSlider->value();
    description.ledColor = m_ledColor;

    emit notifyDescriptionUpdated(m_index, description);
}
