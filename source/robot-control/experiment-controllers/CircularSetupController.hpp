#ifndef CATS2_CIRCULAR_SETUP_CONTROLLER_HPP
#define CATS2_CIRCULAR_SETUP_CONTROLLER_HPP

#include "ExperimentController.hpp"
#include "settings/CircularSetupControllerSettings.hpp"

enum class TurningDirection {
    CLOCK_WISE,
    COUNTER_CLOCK_WISE,
    UNDEFINED
};

/*!
 * The parent class for controllers that implements various behavior on the
 * circular setup.
 * NOTE: we assume that the id's of the control areas in the control map file
 * are sorted in the clock-wise order.
 */
class CircularSetupController : public ExperimentController
{
    Q_OBJECT
public:
    //! Constructor. Gets the robot and the settings.
    CircularSetupController(FishBot* robot,
                            ExperimentControllerType::Enum type,
                            ExperimentControllerSettingsPtr settings);

protected:
    //! Calculates the turning directions of the fish group.
    void computeFishTurningDirection();
    //! Computes the target for the robot based on the target turning direction.
    PositionMeters computeTargetPosition();

private:
    //! Compares two area ids, returns 1 if first is bigger, -1 if it is smaller
    //! and 0 if they are the same.
    int compareAreaIds(QString areaId1, QString areaId2);

protected:
    //! The settings for this controller.
    CircularSetupControllerSettingsData m_settings;

protected:
    //! The current target turning direction of the robot.
    TurningDirection m_fishGroupTurningDirection;
    //! The fish area id in the previous step.
    QString m_previousFishAreaId;
    //! The min area id.
    QString m_minAreaId;
    //! The max area id.
    QString m_maxAreaId;

    //! The target turning direction for the robot. It's defined either by the
    //! fish turning direction or set from outside
    TurningDirection m_targetTurningDirection;
    //! The center of the circular setup (must be specified in the settings).
    PositionMeters m_setupCenter;
};

#endif // CATS2_CIRCULAR_SETUP_CONTROLLER_HPP
