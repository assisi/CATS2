#ifndef CATS2_CIRCULAR_SETUP_CONTROLLER_HPP
#define CATS2_CIRCULAR_SETUP_CONTROLLER_HPP

#include "ExperimentController.hpp"
#include "settings/CircularSetupControllerSettings.hpp"

#include "QtCore/QTimer"

class TurningDirection {
public:
    enum Enum {
        CLOCK_WISE,
        COUNTER_CLOCK_WISE,
        UNDEFINED
    };

    //! Returns the turning direction string in a human friendly format.
    static QString toString(Enum direction)
    {
        QString string = "Undef";

        switch (direction) {
        case CLOCK_WISE:
            string = " CW";
            break;
        case COUNTER_CLOCK_WISE:
            string = "CCW";
            break;
        default:
            break;
        }
        return string;
    }

    //! Converts the turning direction string from the human friendly format,
    //! expected values are "CW" or "CCW".
    static Enum fromString(QString direction)
    {
        if (direction.toUpper() == "CW")
            return Enum::CLOCK_WISE;
        else if (direction.toUpper() == "CCW")
            return Enum::COUNTER_CLOCK_WISE;
        else
            return Enum::UNDEFINED;
    }
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
    explicit CircularSetupController(FishBot* robot,
                            ExperimentControllerSettingsPtr settings,
                            ExperimentControllerType::Enum controllerType);
    //! Destructor.
    virtual ~CircularSetupController();

public:
    //! Called when the controller is activated. Used to reset parameters.
    virtual void start() override;
    //! Called when the controller is disactivated.
    virtual void finish() override;

signals:
    //! Informs the interspecies-module on the fish and robots turning directions.
    void notifyTurningDirections(QString fishTurningDirection,
                                 QString robotTurningDirection);

    void notifyStatisticsAvailable(double fishClockWisePercent,
                                   double fishCounterClockWisePercent,
                                   TurningDirection::Enum fishTurningDirection,
                                   double robotClockWisePercent,
                                   double robotCounterClockWisePercent
                                   TurningDirection::Enum robotTurningDirection);

protected:
    //! Calculates the turning directions of the fish group.
    void computeFishTurningDirection();
    //! Computes the target for the robot based on the target turning direction.
    PositionMeters computeTargetPosition();
    //! Updates the current target turning direction for the robot. Returns true
    //! if the turning direction changes.
    bool updateTargetTurningDirection(TurningDirection::Enum turningDirection);
    //! Update the turning directions statistics.
    void updateStatistics();

private:
    //! Compares two area ids, returns 1 if first is bigger, -1 if it is smaller
    //! and 0 if they are the same.
    int compareAreaIds(QString areaId1, QString areaId2);
    //! Prints the turning directions statistics.
    void printStatistics(bool final = false);

protected:
    //! The settings for this controller.
    CircularSetupControllerSettingsData m_settings;

protected:
    //! The current target turning direction of the robot.
    TurningDirection::Enum m_fishGroupTurningDirection;
    //! The fish area id in the previous step.
    QString m_previousFishAreaId;
    //! The min area id.
    QString m_minAreaId;
    //! The max area id.
    QString m_maxAreaId;

    //! The target turning direction for the robot. It's defined either by the
    //! fish turning direction or set from outside
    TurningDirection::Enum m_targetTurningDirection;
    //! The center of the circular setup (must be specified in the settings).
    PositionMeters m_setupCenter;

    //! Counts the measements when the robot was going clock-wise.
    int m_robotClockWiseCounter;
    //! Counts the measements when the robot turning direction was unknown.
    int m_robotUndefCounter;
    //! Counts the measements when the fish was going clock-wise.
    int m_fishClockWiseCounter;
    //! Counts the measements when the robot turning direction was unknown.
    int m_fishUndefCounter;
    //! Counts all measements.
    int m_allMeasurementsCounter;

private:
    //! The timer to print the experiment statistics every minute.
    QTimer m_statisticsPrintTimer;

    //! The timer to publish statistics
    QTimer m_statisticsPublisherTimer;
};

#endif // CATS2_CIRCULAR_SETUP_CONTROLLER_HPP
