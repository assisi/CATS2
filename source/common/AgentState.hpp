#ifndef CATS2_POSITION_HPP
#define CATS2_POSITION_HPP

#include <QtCore/QString>
#include <QtCore/QObject>

/*
 * The position and orientation classes that define the agent's state.
 */
/*!
 * \brief The class that stores the orientation in radians.
 * NOTE : it would me ideologically more correct to have two classes : OrientationImageRad and
 * OrientationWorldRad to not eventually mix the things up.
 */
class OrientationRad
{
public:
    //! Constructor.
    explicit OrientationRad(double angle = 0, bool valid = true): m_angle(angle), m_valid(valid) { }

    //! Sets the x coordinate.
    void setAngle(double angle) { m_angle = angle; }
    //! Return the x coordinate.
    double angle() const { return m_angle; }

    //! Set the validity status.
    void setValid(bool valid) { m_valid = valid; }
    //! Return the position validity status.
    bool isValid() const { return m_valid; }

private:
    //! The angle value.
    double m_angle;  // [Rad]
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
            return QObject::tr("%1 m, %2 m").arg(m_x, 0, 'f', 3).arg(m_y, 0, 'f', 3);
        else
            return QObject::tr("- m, - m");
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
};

/*!
 * \brief The class that stores the position in frame pixels.
 * Use CoordiantesTransformation class to convert the position in pixels to
 * the position in meters.
 */
class PositionPixels
{
public:
    //! Constructor.
    PositionPixels(double x = 0, double y = 0, bool valid = true) : m_x(x), m_y(y), m_valid(valid) { }

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

    //! Sets the position.
    void setPosition(PositionPixels position) { m_positionPixels = position; }
    //! Return the position.
    PositionPixels position() const { return m_positionPixels; }

    //! Sets the orientation.
    void setOrientation(OrientationRad orientationRad) { m_orientationRad = orientationRad; }
    //! Return the orientation.
    OrientationRad orientation() const { return m_orientationRad; }

private:
    //! Position in meters.
    PositionPixels m_positionPixels;  // [m]
    //! The orientation of the agent, with respect to the horizontal x axis.
    OrientationRad m_orientationRad;
};

#endif // CATS2_POSITION_HPP
