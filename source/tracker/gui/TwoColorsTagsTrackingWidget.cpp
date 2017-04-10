#include "TwoColorsTagsTrackingWidget.hpp"
#include "ui_TwoColorsTagsTrackingWidget.h"

#include <settings/TwoColorsTagTrackingSettings.hpp>
#include <routines/TwoColorsTagTracking.hpp>

#include <QtWidgets/QSpinBox>
#include <QtCore/QDebug>
#include <QtGui/QColor>
#include <QtWidgets/QColorDialog>

/*!
 * Constructor.
 */
TwoColorsTagsTrackingWidget::TwoColorsTagsTrackingWidget(TrackingRoutinePtr routine, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::TwoColorsTagsTrackingWidget),
    m_routine(routine)
{
    m_ui->setupUi(this);

    // fill the gui from the routine's settings
    TwoColorsTagTracking* twoColorsTracking = dynamic_cast<TwoColorsTagTracking*>(m_routine.data());
    if (twoColorsTracking) {
        const TwoColorsTagTrackingSettingsData& settings = twoColorsTracking->settings();

        m_ui->centerPositionEdit->setText(QString::number(settings.centerProportionalPosition(), 'f', 4));
        m_ui->centerPositionEdit->setEnabled(false);

        // get the back tag description
        TwoColorsTagTrackingSettingsData::TagGroupDescription description =
            settings.tagGroupDescription(TwoColorsTagTrackingSettingsData::TagType::BACK);
        m_ui->backTagsNumberSpinBox->setValue(description.numberOfTags);
        m_ui->backTagColorValueLabel->setText(QString("(%1,%2,%3)")
                                              .arg(description.tagColor.red())
                                              .arg(description.tagColor.green())
                                              .arg(description.tagColor.blue()));
        m_ui->backThresholdValueLabel->setText(QString::number(description.colorThreshold));
        m_ui->backThresholdSlider->setValue(description.colorThreshold);
        m_backColor = description.tagColor;

        // get the front tag description
        description =
            settings.tagGroupDescription(TwoColorsTagTrackingSettingsData::TagType::FRONT);
        m_ui->frontTagsNumberSpinBox->setValue(description.numberOfTags);
        m_ui->frontTagColorValueLabel->setText(QString("(%1,%2,%3)")
                                              .arg(description.tagColor.red())
                                              .arg(description.tagColor.green())
                                              .arg(description.tagColor.blue()));
        m_ui->frontThresholdValueLabel->setText(QString::number(description.colorThreshold));
        m_ui->frontThresholdSlider->setValue(description.colorThreshold);
        m_frontColor = description.tagColor;
    } else {
        qDebug() << "The tracking routine is ill-defined";
    }

    connect(m_ui->frontThresholdSlider,
            static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged),
            this, &TwoColorsTagsTrackingWidget::updateSettings);
    connect(m_ui->frontThresholdSlider,
            static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged),
            [=](int value) {
                m_ui->frontThresholdValueLabel->setText(QString::number(value));
            });
    connect(m_ui->backThresholdSlider,
            static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged),
            this, &TwoColorsTagsTrackingWidget::updateSettings);
    connect(m_ui->backThresholdSlider,
            static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged),
            [=](int value) {
                m_ui->backThresholdValueLabel->setText(QString::number(value));
            });

    connect(m_ui->backTagColorButton, &QPushButton::clicked,
            [=]() {
                QColor color = QColorDialog::getColor(m_backColor, this);
                if (color.isValid()) {
                    m_backColor = color;
                    updateSettings();

                    m_ui->backTagColorValueLabel->setText(QString("(%1,%2,%3)")
                                                          .arg(m_backColor.red())
                                                          .arg(m_backColor.green())
                                                          .arg(m_backColor.blue()));
                }
            });
    connect(m_ui->frontTagColorButton, &QPushButton::clicked,
            [=]() {
                QColor color = QColorDialog::getColor(m_frontColor, this);
                if (color.isValid()) {
                    m_frontColor = color;
                    updateSettings();

                    m_ui->frontTagColorValueLabel->setText(QString("(%1,%2,%3)")
                                                          .arg(m_frontColor.red())
                                                          .arg(m_frontColor.green())
                                                          .arg(m_frontColor.blue()));
                }
            });
}

/*!
 * Destructor.
 */
TwoColorsTagsTrackingWidget::~TwoColorsTagsTrackingWidget()
{
    qDebug() << "Destroying the object";
    delete m_ui;
}

/*!
 * Triggered when the settings are modified.
 */
void TwoColorsTagsTrackingWidget::updateSettings()
{
    TwoColorsTagTrackingSettingsData updatedSettings;

    // update back tag data
    TwoColorsTagTrackingSettingsData::TagGroupDescription description;
    description.numberOfTags = m_ui->backTagsNumberSpinBox->value();
    description.colorThreshold = m_ui->backThresholdSlider->value();
    description.tagColor = m_backColor;
    updatedSettings.updateTagGroupDescription(TwoColorsTagTrackingSettingsData::TagType::BACK,
                                              description);

    // update front tag data
    description.numberOfTags = m_ui->frontTagsNumberSpinBox->value();
    description.colorThreshold = m_ui->frontThresholdSlider->value();
    description.tagColor = m_frontColor;
    updatedSettings.updateTagGroupDescription(TwoColorsTagTrackingSettingsData::TagType::FRONT,
                                              description);


    TwoColorsTagTracking* twoColorsTracking = dynamic_cast<TwoColorsTagTracking*>(m_routine.data());
    if (twoColorsTracking) {
        twoColorsTracking->setSettings(updatedSettings);
    } else {
        qDebug() << "The tracking routine is ill-defined";
    }
}
