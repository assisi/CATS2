#ifndef CATS2_EXPERIMENT_CONTROLLER_HPP
#define CATS2_EXPERIMENT_CONTROLLER_HPP

#include "ExperimentControllerType.hpp"
#include "ControlArea.hpp"

#include <QtCore/QObject>

class FishBot;

/*!
 * The parent class for the experiment controller. Reads the control areas from
 * a XML file, stores it and returns for every point the corresponding control
 * data or undefined if it is not contained by any area. The control data is
 * either predefined by the control map or computed by the controllers. Control
 * areas should covers the whole experimental area.
 */
class ExperimentController : public QObject
{
    Q_OBJECT
public:
    //! Constructor. It gets a pointer to the robot that is controlled by this
    //! controller.
    ExperimentController(FishBot* robot,
                         QString controlAreasFileName,
                         ExperimentControllerType::Enum type = ExperimentControllerType::NONE);

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

protected:
    //! A pointer to the robot that is controlled by this controller.
    FishBot* m_robot;
    //! A list of control areas.
    QList<ControlArea> m_controlAreas;

private:
    //! Reads the control map from a file.
    bool deserialize(QString controlAreasFileName);

private:
    //! A type of the controller.
    ExperimentControllerType::Enum m_type;
    //! A flag that says if the contoller was correctly initialized.
    bool m_valid;
};

#endif // CATS2_EXPERIMENT_CONTROLLER_HPP
