#include "PotentialFieldWidget.hpp"
#include "ui_PotentialFieldWidget.h"

#include "navigation/PotentialField.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
PotentialFieldWidget::PotentialFieldWidget(PotentialFieldPtr obstacleAvoidanceRoutine, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::PotentialFieldWidget),
    m_routine(obstacleAvoidanceRoutine)
{
    m_ui->setupUi(this);

    // fill the gui from the routine's settings
    PotentialFieldSettings settings = obstacleAvoidanceRoutine->settings();
    m_ui->influenceDistanceArenaSpinBox->setValue(settings.influenceDistanceArenaMeters);
    m_ui->influenceStrengthArenaSpinBox->setValue(settings.influenceStrengthArena);
    m_ui->influenceDistanceRobotsSpinBox->setValue(settings.influenceDistanceRobotsMeters);
    m_ui->influenceStrengthRobotsSpinBox->setValue(settings.influenceStrengthRobots);
    m_ui->influenceDistanceTargetSpinBox->setValue(settings.influenceDistanceTargetMeters);
    m_ui->influenceStrengthTargetSpinBox->setValue(settings.influenceStrengthTarget);
    m_ui->maxAngleDegSpinBox->setValue(settings.maxAngleDeg);
    m_ui->maxForceSpinBox->setValue(settings.maxForce);
    m_ui->obstacleAvoidanceAreaDiameterSpinBox->setValue(settings.obstacleAvoidanceAreaDiameterMeters);

    connect(m_ui->influenceDistanceArenaSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &PotentialFieldWidget::updateSettings);
    connect(m_ui->influenceStrengthArenaSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &PotentialFieldWidget::updateSettings);
    connect(m_ui->influenceDistanceRobotsSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &PotentialFieldWidget::updateSettings);
    connect(m_ui->influenceStrengthRobotsSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &PotentialFieldWidget::updateSettings);
    connect(m_ui->influenceDistanceTargetSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &PotentialFieldWidget::updateSettings);
    connect(m_ui->influenceStrengthTargetSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &PotentialFieldWidget::updateSettings);
    connect(m_ui->maxAngleDegSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &PotentialFieldWidget::updateSettings);
    connect(m_ui->maxForceSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &PotentialFieldWidget::updateSettings);
    connect(m_ui->obstacleAvoidanceAreaDiameterSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &PotentialFieldWidget::updateSettings);
}

/*!
 * Destructor.
 */
PotentialFieldWidget::~PotentialFieldWidget()
{
    qDebug() << "Destroying the object";
    delete m_ui;
}

/*!
 * Triggered when the settings are modified.
 */
void PotentialFieldWidget::updateSettings()
{
    PotentialFieldSettings updatedSettings;
    updatedSettings.influenceDistanceArenaMeters = m_ui->influenceDistanceArenaSpinBox->value();
    updatedSettings.influenceDistanceRobotsMeters = m_ui->influenceDistanceRobotsSpinBox->value();
    updatedSettings.influenceDistanceTargetMeters = m_ui->influenceDistanceTargetSpinBox->value();

    updatedSettings.influenceStrengthArena = m_ui->influenceStrengthArenaSpinBox->value();
    updatedSettings.influenceStrengthRobots = m_ui->influenceStrengthRobotsSpinBox->value();
    updatedSettings.influenceStrengthTarget = m_ui->influenceStrengthTargetSpinBox->value();

    updatedSettings.maxAngleDeg = m_ui->maxForceSpinBox->value();
    updatedSettings.maxForce = m_ui->maxForceSpinBox->value();
    updatedSettings.obstacleAvoidanceAreaDiameterMeters = m_ui->obstacleAvoidanceAreaDiameterSpinBox->value();

    m_routine->setSettings(updatedSettings);
}

