#include "RobotControlWidget.hpp"
#include "ui_RobotControlWidget.h"
#include "control-modes/ControlModeType.hpp"
#include "FishBot.hpp"

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
    m_ui->controlMapCheckBox->setEnabled(false); // temporary until it's implemented
    // disable controls when the map-based control is active
    connect(m_ui->controlMapCheckBox, &QCheckBox::toggled, [=](bool checked){ m_ui->controlModeComboBox->setEnabled(!checked);
                                                                              m_ui->navigationComboBox->setEnabled(!checked);});

    // fill the control modes
    QList<ControlModeType::Enum> controlModeTypes = robot->supportedControlModes();
    foreach (ControlModeType::Enum type, controlModeTypes) {
        m_ui->controlModeComboBox->addItem(ControlModeType::toString(type), type);
    }
    // set the robot's control mode when it is changed in the combobox
    connect(m_ui->controlModeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=](int index){ m_robot->setControlMode(static_cast<ControlModeType::Enum>(m_ui->controlModeComboBox->currentData().toInt())); });
    // set the control mode from the robot
    connect(m_robot.data(), &FishBot::notifyControlModeChanged,
            [=](ControlModeType::Enum type)
            {
                QString controlModeString = ControlModeType::toString(type);
                if (m_ui->controlModeComboBox->currentText() != controlModeString)
                    m_ui->controlModeComboBox->setCurrentText(controlModeString);
            });

    // fill the navigation type
    // TODO : to implement
}

/*!
 * Destructor.
 */
RobotControlWidget::~RobotControlWidget()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
    delete m_ui;
}


