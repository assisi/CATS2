#include "BlobDetectorWidget.hpp"
#include "ui_BlobDetectorWidget.h"

#include <settings/BlobDetectorSettings.hpp>
#include <routines/BlobDetector.hpp>

#include <QtWidgets/QSpinBox>
#include <QtCore/QDebug>

/*!
 * Constructor.
 */
BlobDetectorWidget::BlobDetectorWidget(TrackingRoutinePtr routine, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::BlobDetectorWidget),
    m_routine(routine)
{
    m_ui->setupUi(this);

    // fill the gui from the routine's settings
    BlobDetector* blobDetector = dynamic_cast<BlobDetector*>(m_routine.data());
    if (blobDetector) {
        const BlobDetectorSettingsData& settings = blobDetector->settings();
        m_ui->numberOfAgentsEdit->setValue(settings.numberOfAgents());
        m_ui->minBlobSizeSpinBox->setValue(settings.minBlobSizePx());
        m_ui->qualityLevelEdit->setText(QString::number(settings.qualityLevel(), 'f', 4));
        m_ui->minDistanceSpinBox->setValue(settings.minDistance());
        m_ui->blockSizeSpinBox->setValue(settings.blockSize());
        m_ui->kEdit->setText(QString::number(settings.k(), 'f', 4));
        m_ui->useHarrisDetectorCheckBox->setChecked(settings.useHarrisDetector());
    } else {
        qDebug() << Q_FUNC_INFO << "The tracking routine is ill-defined";
    }

    connect(m_ui->minBlobSizeSpinBox,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &BlobDetectorWidget::updateSettings);
    connect(m_ui->minDistanceSpinBox,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &BlobDetectorWidget::updateSettings);
    connect(m_ui->blockSizeSpinBox,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &BlobDetectorWidget::updateSettings);

    connect(m_ui->qualityLevelEdit, &QLineEdit::textChanged,
            this, &BlobDetectorWidget::updateSettings);
    connect(m_ui->kEdit, &QLineEdit::textChanged, this,
            &BlobDetectorWidget::updateSettings);

    connect(m_ui->useHarrisDetectorCheckBox, &QCheckBox::toggled,
            this, &BlobDetectorWidget::updateSettings);
}

/*!
 * Destructor.
 */
BlobDetectorWidget::~BlobDetectorWidget()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
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

    BlobDetector* blobDetector = dynamic_cast<BlobDetector*>(m_routine.data());
    if (blobDetector) {
        blobDetector->setSettings(updatedSettings);
    } else {
        qDebug() << Q_FUNC_INFO << "The tracking routine is ill-defined";
    }
}
