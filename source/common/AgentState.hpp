#ifndef CATS2_POSITION_HPP
#define CATS2_POSITION_HPP

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
    explicit OrientationRad(double angle = 0, bool valid = true): _angle(angle), _valid(valid) { }

    //! Sets the x coordinate.
    void setAngle(double angle) { _angle = angle; }
    //! Return the x coordinate.
    double angle() { return _angle; }

    //! Set the validity status.
    void setValid(bool valid) { _valid = valid; }
    //! Return the position validity status.
    bool isValid() { return _valid; }

private:
    //! The angle value.
    double _angle;  // [Rad]
    //! The angle validity, set to false when the angle could not be determined.
    bool _valid;
};

/*!
 * \brief The class that stores the position in meters.
 */
class PositionMeters
{
public:
    //! Constructor.
    explicit PositionMeters(double x = 0, double y = 0, double z = 0, bool valid = true): _x(x), _y(y), _z(z), _valid(valid) { }

    //! Sets the x coordinate.
    void setX(double x) { _x = x; }
    //! Return the x coordinate.
    double x() { return _x; }

    //! Sets the y coordinate.
    void setY(double y) { _y = y; }
    //! Return the y coordinate.
    double y() { return _y; }

    //! Set the validity status.
    void setValid(bool valid) { _valid = valid; }
    //! Return the position validity status.
    bool isValid() { return _valid; }

private:
    //! Position x.
    double _x;  // [m]
    //! Position y.
    double _y;  // [m]
    //! Position z.
    double _z; // [m]
    //! Position validity, set to false when the position could not be determined.
    bool _valid;
};

/*!
 * \brief The class that stores the position in meters and the orientation.
 */
class StateMeters
{
public:
    //! Constructor.
    StateMeters(PositionMeters position = PositionMeters(), OrientationRad orientation = OrientationRad()) :
        _positionMeters(position),
        _orientationRad(orientation)
    {
    }

    //! Sets the position.
    void setPosition(PositionMeters position) { _positionMeters = position; }
    //! Return the position.
    PositionMeters position() { return _positionMeters; }

    //! Sets the orientation.
    void setOrientation(OrientationRad orientationRad) { _orientationRad = orientationRad; }
    //! Return the orientation.
    OrientationRad orientation() { return _orientationRad; }

private:
    //! Position in meters.
    PositionMeters _positionMeters;  // [m]
    //! The orientation of the agent, with respect to the horizontal x axis.
    OrientationRad _orientationRad;
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
    PositionPixels(double x = 0, double y = 0, bool valid = true) : _x(x), _y(y), _valid(valid) { }

    //! Sets the x coordinate.
    void setX(double x) { _x = x; }
    //! Return the x coordinate.
    double x() { return _x; }

    //! Sets the y coordinate.
    void setY(double y) { _y = y; }
    //! Return the y coordinate.
    double y() { return _y; }

    //! Set the validity status.
    void setValid(bool valid) { _valid = valid; }
    //! Return the position validity status.
    bool isValid() { return _valid; }

private:
    //! Position x.
    double _x;  // [pixels]
    //! Position y.
    double _y;  // [pixels]
    //! Position validity, set to false when the position could not be determined.
    bool _valid;
};


#endif // CATS2_POSITION_HPP
