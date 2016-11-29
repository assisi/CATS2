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
 * Describes an area of the control map defined by the same control behavior.
 */
class ControlArea
{
public:
    //! Constructor.
    ControlArea(QString label);

public:
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
    void addPolygon(QPolygonF polygon) { m_polygons.append(polygon); }
    //! Adds a polygon included in this area.
    void addPolygon(std::vector<cv::Point2f>);
    //! Checks if the point is inside this area.
    bool contains(QPointF) const;

    //! Returns the polygons to be used in gui.
    AnnotatedPolygons annotatedPolygons() const;

private:
    //! Area's label.
    QString m_label;
    //! Area's color (for gui).
    QColor m_color;
    //! The control mode corresponding to this area.
    ControlModeType::Enum m_controlMode;
    //! The motion pattern corresponding to this area.
    MotionPatternType::Enum m_motionPattern;

    // TODO : it's more logical to use WorldPolygon instead of
    // QPolygonF. But we use QPolygonF because it has already contains()
    // method
    //! Polygons of this area.
    QList<QPolygonF> m_polygons;
};

#endif // CATS2_CONTROL_AREA_HPP
