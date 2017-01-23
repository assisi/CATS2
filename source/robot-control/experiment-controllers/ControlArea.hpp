#ifndef CATS2_CONTROL_AREA_HPP
#define CATS2_CONTROL_AREA_HPP

#include "control-modes/ControlModeType.hpp"
#include "MotionPatternType.hpp"

#include <AgentState.hpp>

#include <QtCore/QList>
#include <QtGui/QColor>
#include <QtGui/QPolygonF>

#include <opencv2/core/types.hpp>

/*!
 * Provides an enum for control area type.
 */
class ControlAreaType
{
public:
    enum Enum {
        CORRIDOR,
        ROOM,
        UNDEFINED
    };

    //! Gets the type of the control area type from the settings' string.
    static Enum fromSettingsString(QString areaTypeName)
    {
        if (areaTypeName.toLower() == "corridor")
            return CORRIDOR;
        else if (areaTypeName.toLower() == "room")
            return ROOM;
        else
            return UNDEFINED;
    }
};

/*!
 * Describes an area of the control map defined by the same control behavior.
 */
class ControlArea
{
public:
    //! Constructor.
    ControlArea(QString id, ControlAreaType::Enum type);

public:
    //! Returns the id.
    QString id() const { return m_id; }
    //! Returns the type.
    ControlAreaType::Enum type() const { return m_type; }

    //! Sets the color.
    void setColor(QColor color) { m_color = color; }
    //! Returns the color.
    QColor color() const { return m_color; }

    //! Sets the control mode.
    void setControlMode(ControlModeType::Enum controlMode) { m_controlMode = controlMode; }
    //! Returns the control mode.
    ControlModeType::Enum controlMode() const { return m_controlMode; }

    //! Sets the motion pattern.
    void setMotionPattern(MotionPatternType::Enum motionPattern) { m_motionPattern = motionPattern; }
    //! Returns the motion pattern.
    MotionPatternType::Enum motionPattern() const { return m_motionPattern; }

    //! Adds a polygon included in this area.
    void addPolygon(WorldPolygon polygon);
    //! Adds a polygon included in this area.
    void addPolygon(std::vector<cv::Point2f>);
    //! Checks if the point is inside this area.
    bool contains(PositionMeters) const;

    //! Returns the polygons to be used in gui.
    AnnotatedPolygons annotatedPolygons() const;

    //! Returns the area centroid.
    PositionMeters centroid() const { return m_centroid; }

private:
    //! Area's label.
    QString m_id;
    //! Area type.
    ControlAreaType::Enum m_type;
    //! Area's color (for gui).
    QColor m_color;
    //! The control mode corresponding to this area.
    ControlModeType::Enum m_controlMode;
    //! The motion pattern corresponding to this area.
    MotionPatternType::Enum m_motionPattern;

    //! Polygons of this area.
    QList<WorldPolygon> m_polygons;

    // TODO : a geometry library is needed
    // FIXME : it's too much of the simplification
    //! Computes the area centroid. For the areas composed of several polygons
    //! only the first polygon is taken into account.
    void updateCentroid();
    //! The area centroid.
    PositionMeters m_centroid;
};

#endif // CATS2_CONTROL_AREA_HPP
