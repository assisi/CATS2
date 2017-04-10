#ifndef CATS2_EXPERIMENT_CONTROLLER_HPP
#define CATS2_EXPERIMENT_CONTROLLER_HPP

#include "ExperimentControllerType.hpp"
#include "RobotControlPointerTypes.hpp"
#include "MotionPatternType.hpp"
#include "control-modes/ControlModeType.hpp"

#include <AgentState.hpp>

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QVariant>

class FishBot;

/*!
 * The parent class for the experiment controller. Reads the control areas from
 * a XML file, stores it and returns for every point the corresponding control
 * data or undefined if it is not contained by any area. The control data is
 * either predefined by the control map or computed by the controllers. Control
 * areas must covers the whole experimental area.
 */
class ExperimentController : public QObject
{
    Q_OBJECT
public:
    //! Constructor. It gets a pointer to the robot that is controlled by this
    //! controller.
    ExperimentController(FishBot* robot,
                         ExperimentControllerType::Enum type = ExperimentControllerType::NONE);

    //! The data from the control map returned on request for given position.
    struct ControlData {
        //! Initialization.
        ControlData() : controlMode(ControlModeType::UNDEFINED),
            motionPattern(MotionPatternType::UNDEFINED) {}
        //! The control mode.
        ControlModeType::Enum controlMode;
        //! The motion pattern.
        MotionPatternType::Enum motionPattern;
        //! The extra control data to send (e.g. target for the go-to-target
        //! control mode).
        QVariant data;
    };

public:
    //! Returns the type of the controller.
    ExperimentControllerType::Enum type() const { return m_type; }

    //! Called when the controller is activated. Used to reset parameters.
    virtual void start() {}
    //! Returns the control values for given position.
    virtual ControlData step();
    //! Called when the controller is disactivated. Makes a cleanup if necessary.
    virtual void finish() {}

public slots:
    //! Requests to sends the map areas' polygons.
    void requestPolygons();

signals:
    //! Sends the map areas' polygons.
    void notifyPolygons(QList<AnnotatedPolygons>);
    //! Sends out the current controller status.
    void notifyControllerStatus(QString status);

protected:
    //! Reads the control map from a file.
    void readControlMap(QString controlAreasFileName);

protected:
    //! Find where the robot and the fish are.
    void updateAreasOccupation();
    //! Finds the room that contains given point.
    bool findAreaByPosition(QString& areaId, const PositionMeters& position);
    //! Finds the room with the majority of fish. Returns the success status.
    bool findFishArea(QString& areaId);
    //! Counts the fish number in all rooms different from the current one.
    int fishNumberInOtherRooms(QString currentAreaId);
    //! Finds the room where the robot is. Returns the success status.
    bool findRobotArea(QString& areaId);

protected:
    //! A pointer to the robot that is controlled by this controller.
    FishBot* m_robot;
    //! Maps control areas' ids to the areas. Since they will be most probably
    //! used by all controller they are placed in this parent class.
    QMap<QString, ControlAreaPtr> m_controlAreas;
    //! Prefered area id.
    QString m_preferedAreaId;
    //! The area with the majority of fish.
    QString m_fishAreaId;
    //! The area where the robot is.
    QString m_robotAreaId;
    //! Where the fish are.
    QMap<QString, int> m_fishNumberByArea;
    //! The flag that shows that the robot's area has changed.
    bool m_robotAreaChanged;

private:
    //! Sets the robot's area.
    void updateRobotArea(QString areaId);

private:
    //! A type of the controller.
    ExperimentControllerType::Enum m_type;
};

#endif // CATS2_EXPERIMENT_CONTROLLER_HPP
