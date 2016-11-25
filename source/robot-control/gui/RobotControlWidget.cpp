#include "RobotControlWidget.hpp"
#include "ui_RobotControlWidget.h"
#include "control-modes/ControlModeType.hpp"
#include "MotionPatternType.hpp"
#include "FishBot.hpp"
#include "settings/RobotControlSettings.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
RobotControlWidget::RobotControlWidget(FishBotPtr robot, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::RobotControlWidget),
    m_robot(robot)
{
    m_ui->setupUi(this);

    // set the robot's name
    m_ui->robotNameLabel->setText(robot->name());

    // set the controls
    // disable controls when the map-based control is active
    connect(m_ui->controlMapCheckBox, &QCheckBox::toggled,
            [=](bool checked){ m_ui->controlModeComboBox->setEnabled(!checked);
                               m_ui->navigationComboBox->setEnabled(!checked);
                               m_ui->frequencyDividerSpinBox->setEnabled(!checked);
                               m_robot->setUseControlMap(checked);
                             });

    // set the robot's control mode when it is changed in the combobox
    connect(m_ui->controlModeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index)
            {
                m_robot->setControlMode(static_cast<ControlModeType::Enum>(m_ui->controlModeComboBox->currentData().toInt()));
                // show the navigation pattern choice when it's supported or
                // hide when it is not supported
                m_ui->navigationWidget->setVisible(m_robot->supportsMotionPatterns());
            });

    // set the control mode from the robot
    connect(m_robot.data(), &FishBot::notifyControlModeChanged,
            [=](ControlModeType::Enum type)
            {
                QString controlModeString = ControlModeType::toString(type);
                if (m_ui->controlModeComboBox->currentText() != controlModeString)
                    m_ui->controlModeComboBox->setCurrentText(controlModeString);
            });

    // fill the control modes
    QList<ControlModeType::Enum> controlModeTypes = robot->supportedControlModes();
    foreach (ControlModeType::Enum type, controlModeTypes) {
        m_ui->controlModeComboBox->addItem(ControlModeType::toString(type), type);
    }
    m_ui->controlModeComboBox->setCurrentText(ControlModeType::toString(m_robot->currentControlMode()));

    // set the robot's motion pattern when it is changed in the combobox
    connect(m_ui->navigationComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index)
            {
                MotionPatternType::Enum motionPattern = static_cast<MotionPatternType::Enum>(m_ui->navigationComboBox->currentData().toInt());
                m_robot->setMotionPattern(motionPattern);
                // frequency divider is used for fish motion pattern only
                m_ui->frequencyDividerSpinBox->setVisible(motionPattern == MotionPatternType::FISH_MOTION);
                m_ui->frequencyDividerLabel->setVisible(motionPattern == MotionPatternType::FISH_MOTION);
                // set the robot's motion pattern frequency divider
                m_ui->frequencyDividerSpinBox->setValue(m_robot->motionPatternFrequencyDivider(motionPattern));
            });

    // set the motion pattern from the robot
    connect(m_robot.data(), &FishBot::notifyMotionPatternChanged,
            [=](MotionPatternType::Enum type)
            {
                QString motionPatternString = MotionPatternType::toString(type);
                if (m_ui->navigationComboBox->currentText() != motionPatternString)
                    m_ui->navigationComboBox->setCurrentText(motionPatternString);
            });

    // fill the navigation type
    for (int type = MotionPatternType::PID; type < MotionPatternType::UNDEFINED; ++type) {
        m_ui->navigationComboBox->addItem(MotionPatternType::toString(static_cast<MotionPatternType::Enum>(type)), type);
    }
    m_ui->navigationComboBox->setCurrentText(MotionPatternType::toString(m_robot->currentMotionPattern()));

    // set the robot's motion pattern frequency divider when it is changed in the gui
    connect(m_ui->frequencyDividerSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int value)
            {
                m_robot->setMotionPatternFrequencyDivider(static_cast<MotionPatternType::Enum>(m_ui->navigationComboBox->currentData().toInt()),
                                                          value);
                // specify the resulted frequency
                m_ui->frequencyDividerSpinBox->setSuffix(QString("[%1Hz]")
                                                         .arg(static_cast<float>(RobotControlSettings::get().controlFrequencyHz()) / value , 0, 'f', 1));
            });

    // set the motion pattern frequency divider from the robot
    connect(m_robot.data(), &FishBot::notifyMotionPatternFrequencyDividerChanged,
            [=](MotionPatternType::Enum type, int value)
            {
                if (m_ui->frequencyDividerSpinBox->value() != value)
                    m_ui->frequencyDividerSpinBox->setValue(value);
            });
}

/*!
 * Destructor.
 */
RobotControlWidget::~RobotControlWidget()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
    delete m_ui;
}


