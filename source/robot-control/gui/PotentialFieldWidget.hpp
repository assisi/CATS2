#ifndef CATS2_POTENTIAL_FIELD_WIDGET_HPP
#define CATS2_POTENTIAL_FIELD_WIDGET_HPP

#include "RobotControlPointerTypes.hpp"

#include <QtWidgets/QWidget>

namespace Ui {
class PotentialFieldWidget;
}

/*!
 * The widget to set parameters of the potential field routine.
 */
class PotentialFieldWidget : public QWidget
{
    Q_OBJECT

public:
    //! Constructor.
    explicit PotentialFieldWidget(PotentialFieldPtr obstacleAvoidanceRoutine, QWidget *parent = nullptr);
    //! Destructor.
    virtual ~PotentialFieldWidget() final;

protected slots:
    //! Triggered when the settings are modified.
    void updateSettings();

private:
    Ui::PotentialFieldWidget *m_ui;
    // TODO : once other obstacle avoidance methods appear make the type more
    // generic
    //! The obstacle avoidance routine.
    PotentialFieldPtr m_routine;
};

#endif // CATS2_POTENTIAL_FIELD_WIDGET_HPP
