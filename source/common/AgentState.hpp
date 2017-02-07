#ifndef CATS2_POSITION_HPP
#define CATS2_POSITION_HPP

#include <opencv2/core/types.hpp>

#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtCore/QtMath>
#include <QtGui/QColor>
#include <QtGui/QPolygonF>

/*
 * The position and orientation classes that define the agent's state.
 */
/*!
 * \brief The class that stores the orientation in the interval [-pi,+pi] radians.
 * NOTE : it would me ideologically more correct to have two classes : OrientationImageRad and
 * OrientationWorldRad to not eventually mix the things up.
 */
class OrientationRad
{
public:
    //! Constructor.
    explicit OrientationRad(double angleRad = 0, bool valid = true): m_angleRad(angleRad), m_valid(valid) { }
    //! Copy constructor.
    OrientationRad(const OrientationRad&) = default;
    //! Copy operator.
    OrientationRad& operator=(const OrientationRad&) = default;
    //! Move operator.
    OrientationRad& operator=(OrientationRad&&) = default;
    //! Destructor.
    ~OrientationRad() = default;

    //! Sets the angle.
    void setAngleRad(double angleRad) { m_angleRad = angleRad; }
    //! Return the angle ([-pi,+pi] radians).
    double angleRad() const { return m_angleRad; }
    //! Return the angle ([-180,+180] degrees).
    double angleDeg() const { return m_angleRad * 180 / M_PI; }

    //! Set the validity status.
    void setValid(bool valid) { m_valid = valid; }
    //! Return the position validity status.
    bool isValid() const { return m_valid; }

private:
    //! The angle value.
    double m_angleRad;  // [-pi,+pi] radians.
    //! The angle validity, set to false when the angle could not be determined.
    bool m_valid;
};

/*!
 * \brief The class that stores the position in meters.
 */
class PositionMeters
{
public:
    //! Constructor.
    explicit PositionMeters(double x = 0, double y = 0, double z = 0, bool valid = true): m_x(x), m_y(y), m_z(z), m_valid(valid) { }
    //! Copy constructor.
    PositionMeters(const PositionMeters&) = default;
    //! Copy operator.
    PositionMeters& operator=(const PositionMeters&) = default;
    //! Move operator.
    PositionMeters& operator=(PositionMeters&&) = default;
    //! Destructor.
    ~PositionMeters() = default;

    //! Sets the x coordinate.
    void setX(double x) { m_x = x; }
    //! Return the x coordinate.
    double x() const { return m_x; }

    //! Sets the y coordinate.
    void setY(double y) { m_y = y; }
    //! Return the y coordinate.
    double y() const { return m_y; }

    //! Sets the z coordinate.
    void setZ(double z) { m_z = z; }
    //! Return the z coordinate.
    double z() const { return m_z; }

    //! Set the validity status.
    void setValid(bool valid) { m_valid = valid; }
    //! Return the position validity status.
    bool isValid() const { return m_valid; }

    //! Returns the position as text. Only x and y coordinates are shown.
    QString toString()
    {
        if (m_valid)
            return QObject::tr("%1 m, %2 m").arg(m_x, 0, 'f', 3).arg(m_y, 0, 'f', 3);
        else
            return QObject::tr("%1 m, %2 m (invalid)").arg(m_x, 0, 'f', 3).arg(m_y, 0, 'f', 3);
    }

    double distanceTo(const PositionMeters& other)
    {
        return qSqrt((m_x - other.x()) * (m_x - other.x()) +
                     (m_y - other.y()) * (m_y - other.y()) +
                     (m_z - other.z()) * (m_z - other.z()));
    }

    /*!
     * Only x and y coordinates are taken into account.
     */
    double distance2DTo(const PositionMeters& other)
    {
        return qSqrt((m_x - other.x()) * (m_x - other.x()) +
                     (m_y - other.y()) * (m_y - other.y()));
    }

    /*!
     * Checks if two points are close in 2D.
     */
    bool closeTo(const PositionMeters& other, double threshold = ProximityThreshold)
    {
        if (m_valid && other.isValid())
            return (distance2DTo(other) < threshold);
        else
            return false;
    }

    /*!
     * Operator +=.
     */
    PositionMeters& operator+=(const PositionMeters &rhs)
    {
        m_x += rhs.x();
        m_y += rhs.y();
        m_z += rhs.z();
        m_valid &= rhs.isValid();
        return *this;
    }

    /*!
     * Operator /=.
     */
    PositionMeters& operator/=(const double &value)
    {
        m_x /= value;
        m_y /= value;
        m_z /= value;
        return *this;
    }

    /*!
     * Return an invalid point.
     */
    static PositionMeters invalidPosition()
    {
        return PositionMeters(0, 0, 0, false);
    }

private:
    //! Position x.
    double m_x;  // [m]
    //! Position y.
    double m_y;  // [m]
    //! Position z.
    double m_z; // [m]
    //! Position validity, set to false when the position could not be determined.
    bool m_valid;

    //! The threshold to decide that one point is close to another. It's used
    //! by in the control modes and navigation.
    // FIXME : it's increased to 10 cm to prevent the robot from blocking around
    // the target, but it's a way too big, need to be reduced once the blocking
    // is resolved
    static constexpr double ProximityThreshold = 0.05; // [m]
};

Q_DECLARE_METATYPE(PositionMeters)

/*!
 * Addition operator.
 */
inline PositionMeters operator+(const PositionMeters& lhs, const PositionMeters& rhs)
{
    return PositionMeters(lhs.x() + rhs.x(), lhs.y() + rhs.y());
}

/*!
 * Subtraction operator.
 */
inline PositionMeters operator-(const PositionMeters& lhs, const PositionMeters& rhs)
{
    return PositionMeters(lhs.x() - rhs.x(), lhs.y() - rhs.y());
}

/*!
 * Comparison operator.
 */
inline bool operator==(const PositionMeters& lhs, const PositionMeters& rhs)
{
    return (qFuzzyCompare(lhs.x(), rhs.x())
            && qFuzzyCompare(lhs.y(), rhs.y())
            && qFuzzyCompare(lhs.z(), rhs.z())
            && (lhs.isValid() == rhs.isValid()));
}

inline bool operator!=(const PositionMeters& lhs, const PositionMeters& rhs)
{
    return !operator==(lhs,rhs);
}

/*!
 * \brief The class that stores the position in frame pixels.
 * Use CoordiantesTransformation class to convert the position in pixels to
 * the position in meters.
 */
class PositionPixels
{
public:
    //! Constructor.
    PositionPixels(cv::Point2f point, bool valid = true) : m_x(point.x), m_y(point.y), m_valid(valid) { }
    //! Constructor.
    PositionPixels(double x = 0, double y = 0, bool valid = true) : m_x(x), m_y(y), m_valid(valid) { }
    //! Copy constructor.
    PositionPixels(const PositionPixels&) = default;
    //! Copy operator.
    PositionPixels& operator=(const PositionPixels&) = default;
    //! Move operator.
    PositionPixels& operator=(PositionPixels&&) = default;
    //! Destructor.
    ~PositionPixels() = default;

    //! Sets the x coordinate.
    void setX(double x) { m_x = x; }
    //! Return the x coordinate.
    double x() const { return m_x; }

    //! Sets the y coordinate.
    void setY(double y) { m_y = y; }
    //! Return the y coordinate.
    double y() const { return m_y; }

    //! Set the validity status.
    void setValid(bool valid) { m_valid = valid; }
    //! Return the position validity status.
    bool isValid() const { return m_valid; }

    //! Returns the position as text.
    QString toString()
    {
        if (m_valid)
            return QObject::tr("%1 px, %2 px").arg(m_x, 0, 'f', 1).arg(m_y, 0, 'f', 1);
        else
            return QObject::tr("- px, - px");
    }

    //! Returns the position as cv::Point2f
    cv::Point2f toCvPoint2f() const
    {
        return cv::Point2f(m_x, m_y);
    }

    //! Returns the point rotates by theta radians.
    PositionPixels rotated(double thetaRad, PositionPixels center) const
    {
        double rotated_x = (m_x - center.x()) * qCos(thetaRad) - (m_y - center.y() / 2) * qSin(thetaRad);
        double rotated_y = (m_x - center.x()) * qSin(thetaRad) + (m_y - center.y() / 2) * qCos(thetaRad);
        return PositionPixels(rotated_x + center.x(), rotated_y + center.y() / 2);
    }

private:
    //! Position x.
    double m_x;  // [pixels]
    //! Position y.
    double m_y;  // [pixels]
    //! Position validity, set to false when the position could not be determined.
    bool m_valid;
};

/*!
 * \brief The class that stores the position in meters and the orientation.
 */
class StateWorld
{
public:
    //! Constructor. If position or orientation are not provided then they are
    //! considered unknown.
    StateWorld(PositionMeters position = PositionMeters(0, 0, 0, false),
               OrientationRad orientation = OrientationRad(0, false)) :
        m_positionMeters(position),
        m_orientationRad(orientation)
    {
    }
    //! Copy constructor.
    StateWorld(const StateWorld&) = default;
    //! Copy operator.
    StateWorld& operator=(const StateWorld&) = default;
    //! Move operator.
    StateWorld& operator=(StateWorld&&) = default;
    //! Destructor.
    ~StateWorld() = default;

    //! Sets the position.
    void setPosition(PositionMeters position) { m_positionMeters = position; }
    //! Return the position.
    PositionMeters position() const { return m_positionMeters; }

    //! Sets the orientation.
    void setOrientation(OrientationRad orientationRad) { m_orientationRad = orientationRad; }
    //! Return the orientation.
    OrientationRad orientation() const { return m_orientationRad; }

private:
    //! Position in meters.
    PositionMeters m_positionMeters;  // [m]
    //! The orientation of the agent, with respect to the horizontal x axis.
    OrientationRad m_orientationRad;
};

/*!
 * \brief The class that stores the position in pixels and the orientation.
 */
class StateImage
{
public:
    //! Constructor. If position or orientation are not provided then they are
    //! considered unknown.
    StateImage(PositionPixels position = PositionPixels(0, 0, false),
               OrientationRad orientation = OrientationRad(0, false)) :
        m_positionPixels(position),
        m_orientationRad(orientation)
    {
    }
    //! Copy constructor.
    StateImage(const StateImage&) = default;
    //! Copy operator.
    StateImage& operator=(const StateImage&) = default;
    //! Move operator.
    StateImage& operator=(StateImage&&) = default;
    //! Destructor.
    ~StateImage() = default;

    //! Invalidates the state.
    void invalidateState()
    {
        m_positionPixels.setValid(false);
        m_orientationRad.setValid(false);
    }
    //! Invalidates the orientation.
    void invalidateOrientation()
    {
        m_orientationRad.setValid(false);
    }

    //! Sets the position.
    void setPosition(PositionPixels position) { m_positionPixels = position; }
    //! Sets the position from the cv::Point2f value.
    void setPosition(cv::Point2f position) { m_positionPixels = PositionPixels(position.x, position.y); }
    //! Return the position.
    PositionPixels position() const { return m_positionPixels; }

    //! Sets the orientation.
    void setOrientation(OrientationRad orientationRad) { m_orientationRad = orientationRad; }
    //! Sets the orientation from the double value.
    void setOrientation(double orientationRad) { m_orientationRad = OrientationRad(orientationRad); }
    //! Return the orientation.
    OrientationRad orientation() const { return m_orientationRad; }

private:
    //! Position in meters.
    PositionPixels m_positionPixels;  // [m]
    //! The orientation of the agent, with respect to the horizontal x axis.
    OrientationRad m_orientationRad;
};

/*!
 * The class for the list of points.
 */
class WorldPolygon : public QList<PositionMeters>
{
public:
    //! Checks if the polygon contains given point.
    // FIXME FIXME FIXME : fast and dirty implementation, redo
    // NOTE : QPolygonF::containsPoint returns true only when the point is
    // _inside_ the polygon, not on the edge
    bool containsPoint(PositionMeters position) const
    {
        QPolygonF polygon;
        for (const PositionMeters& point : *this)
            polygon.append(QPointF(point.x(), point.y()));
        return polygon.containsPoint(QPointF(position.x(), position.y()), Qt::OddEvenFill);
    }
};

//! The structure to store polygons with the corresponding color and
//! the label. Used to store the data to be drawn on the gui.
struct AnnotatedPolygons {
    //! Initialization.
    AnnotatedPolygons()
        : label(""),
          color(Qt::black),
          polygons()
    {}
    //! The label.
    QString label;
    //! The color.
    QColor color;
    //! Polygons.
    QList<WorldPolygon> polygons;
};

Q_DECLARE_METATYPE(AnnotatedPolygons)

#endif // CATS2_POSITION_HPP
