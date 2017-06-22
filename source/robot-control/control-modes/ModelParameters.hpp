#ifndef CATS2_MODEL_PARAMETERS_HPP
#define CATS2_MODEL_PARAMETERS_HPP

#include <QtCore/QObject>

/*!
 * The parameters of the model that can be set from outside.
 */
struct ModelParameters
{
    //! Defines if the model is aware of the presense of fish and receives their
    //! positions.
    bool ignoreFish;
    //! Defines if the model receives the position of the robot.
    bool ignoreRobot;
    //! Constructor.
    ModelParameters(): ignoreFish(false), ignoreRobot(false) { }
};

Q_DECLARE_METATYPE(ModelParameters)

/*!
 * Comparison operator.
 */
inline bool operator==(const ModelParameters& lhs, const ModelParameters& rhs)
{
    return ((lhs.ignoreFish == rhs.ignoreFish) &&
            (lhs.ignoreRobot == rhs.ignoreRobot));
}

/*!
 * Non-equality operator.
 */
inline bool operator!=(const ModelParameters& lhs, const ModelParameters& rhs)
{
    return ((lhs.ignoreFish != rhs.ignoreFish) ||
            (lhs.ignoreRobot == rhs.ignoreRobot));
}

#endif // CATS2_MODEL_PARAMETERS_HPP

