#include "BlobDetectorWidget.hpp"
#include "ui_BlobDetectorWidget.h"

#include <settings/BlobDetectorSettings.hpp>
#include <routines/BlobDetector.hpp>

#include <QtWidgets/QSpinBox>

/*!
 * Constructor.
 */
BlobDetectorWidget::BlobDetectorWidget(BlobDetector* routine, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::BlobDetectorWidget),
    m_routine(routine)
{
    m_ui->setupUi(this);

    // fill the gui from the routine's settings
    const BlobDetectorSettingsData& settings = routine->settings();

    m_ui->numberOfAgentsEdit->setValue(settings.numberOfAgents());
    m_ui->minBlobSizeSpinBox->setValue(settings.minBlobSizePx());
    m_ui->qualityLevelEdit->setText(QString::number(settings.qualityLevel(), 'f', 4));
    m_ui->minDistanceSpinBox->setValue(settings.minDistance());
    m_ui->blockSizeSpinBox->setValue(settings.blockSize());
    m_ui->kEdit->setText(QString::number(settings.k(), 'f', 4));
    m_ui->useHarrisDetectorCheckBox->setChecked(settings.useHarrisDetector());

    connect(m_ui->minBlobSizeSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &BlobDetectorWidget::updateSettings);
    connect(m_ui->minDistanceSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &BlobDetectorWidget::updateSettings);
    connect(m_ui->blockSizeSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &BlobDetectorWidget::updateSettings);

    connect(m_ui->qualityLevelEdit, &QLineEdit::textChanged, this, &BlobDetectorWidget::updateSettings);
    connect(m_ui->kEdit, &QLineEdit::textChanged, this, &BlobDetectorWidget::updateSettings);

    connect(m_ui->useHarrisDetectorCheckBox, &QCheckBox::toggled, this, &BlobDetectorWidget::updateSettings);
}

/*!
 * Destructor.
 */
BlobDetectorWidget::~BlobDetectorWidget()
{
    delete m_ui;
}

/*!
 * Triggered when the settings are modified.
 */
void BlobDetectorWidget::updateSettings()
{
    BlobDetectorSettingsData updatedSettings;

    updatedSettings.setMinBlobSizePx(m_ui->minBlobSizeSpinBox->value());
    updatedSettings.setMinDistance(m_ui->minDistanceSpinBox->value());
    updatedSettings.setBlockSize(m_ui->blockSizeSpinBox->value());

    updatedSettings.setQualityLevel(m_ui->qualityLevelEdit->text().toDouble());
    updatedSettings.setK(m_ui->kEdit->text().toDouble());

    updatedSettings.setUseHarrisDetector(m_ui->useHarrisDetectorCheckBox->isChecked());

    m_routine->setSettings(updatedSettings);
}
