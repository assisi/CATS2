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
};

Q_DECLARE_METATYPE(ModelParameters)

/*!
 * Comparison operator.
 */
inline bool operator==(const ModelParameters& lhs, const ModelParameters& rhs)
{
    return (lhs.ignoreFish == rhs.ignoreFish);
}

/*!
 * Non-equality operator.
 */
inline bool operator!=(const ModelParameters& lhs, const ModelParameters& rhs)
{
    return !(lhs.ignoreFish == rhs.ignoreFish);
}

#endif // CATS2_MODEL_PARAMETERS_HPP

