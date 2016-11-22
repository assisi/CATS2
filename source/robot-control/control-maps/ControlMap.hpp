#ifndef CATS2_CONTROL_MAP_HPP
#define CATS2_CONTROL_MAP_HPP

#include "ControlArea.hpp"

#include <AgentState.hpp>

#include <QtCore/QString>

/*!
 * Reads the whole control map from a XML file, stores it and returns for every
 * point either the corresponding control data or undefined if it is not
 * contained by any area. ControlMap should covers the whole experimental area.
 * The control map for every given position of the experimental setup provides
 * the control mode and motion pattern to excecute in this position. Every robot
 * has it's own control map that it consults on every step.
 */
class ControlMap : public QObject
{
    Q_OBJECT
public:
    //! Constructor. Gets the file name containing the control map description.
    ControlMap(QString controlMapFileName);

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
    //! Returns the control values for given position. If it's not in the map
    //! then default (UNDEFINED) values are returned.
    ControlData controlDataAtPosition(PositionMeters position);

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
    //! A flag that says if the contol map was correctly initialized.
    bool m_valid;
    //! A list of control areas.
    QList<ControlArea> m_controlAreas;
};

#endif // CATS2_CONTROL_MAP_HPP
