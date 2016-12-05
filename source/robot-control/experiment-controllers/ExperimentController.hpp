#ifndef CATS2_EXPERIMENT_CONTROLLER_HPP
#define CATS2_EXPERIMENT_CONTROLLER_HPP

#include "ExperimentControllerType.hpp"
#include "control-maps/ControlArea.hpp"

#include <QtCore/QObject>

class FishBot;

/*!
 * The parent class for the experiment controller. Reads the control areas from
 * a XML file, stores it and returns for every point the corresponding control
 * data or undefined if it is not contained by any area. The control data is
 * either predefined by the control map or computed by the controllers. Control
 * areas should covers the whole experimental area.
 * The control map for every given position of the experimental setup provides
 * the control mode and motion pattern to excecute in this position. Every robot
 * has it's own control map that it consults on every step.
 */
class ExperimentController : public QObject
{
    Q_OBJECT
public:
    //! Constructor. It gets a pointer to the robot that is controlled by this
    //! controller.
    ExperimentController(FishBot* robot,
                         ExperimentControllerType::Enum type,
                         QString controlMapFileName);

    //! The data from the control map returned on request for given position.
    struct ControlData {
        //! Initialization.
        ControlData() : controlMode(ControlModeType::UNDEFINED), motionPattern(MotionPatternType::UNDEFINED) {}
        //! The control mode.
        ControlModeType::Enum controlMode;
        //! The motion pattern.
        MotionPatternType::Enum motionPattern;
    };

public:
    //! Returns the validity flag.
    bool isValid() const { return m_valid; }

    //! Called when the controller is activated. Used to reset parameters.
    virtual void start() {}
    //! Returns the control values for given position.
    virtual ControlData step(PositionMeters position);
    //! Called when the controller is disactivated. Makes a cleanup if necessary.
    virtual void finish() {}

public slots:
    //! Requests to sends the map areas' polygons.
    void requestPolygons();

signals:
    //! Sends the map areas' polygons.
    void notifyPolygons(QList<AnnotatedPolygons>);

private:
    //! Reads the control map from a file.
    bool deserialize(QString controlMapFileName);

private:
    //! A pointer to the robot that is controlled by this controller.
    FishBot* m_robot;
    //! A type of the controller.
    ExperimentControllerType::Enum m_type;
    //! A flag that says if the contoller was correctly initialized.
    bool m_valid;
    //! A list of control areas.
    QList<ControlArea> m_controlAreas;
};

#endif // CATS2_EXPERIMENT_CONTROLLER_HPP
