#ifndef CATS2_MODEL_BASED_HPP
#define CATS2_MODEL_BASED_HPP

#include "ControlMode.hpp"
#include <AgentState.hpp>

/*!
 * \brief The temporary model class
 */
class Model
{
public:
    //! Constructor.
    Model() {}

    //! The target position as requested by the model.
    PositionMeters computeTarget(QList<StateWorld>) { return PositionMeters(0, 0); }
};

/*!
 * Makes the robots to follow the fish mode.
 */
class ModelBased : public ControlMode
{
    Q_OBJECT
public:
    //! Constructor.
    ModelBased(FishBot* robot);

    //! The step of the control mode.
    virtual ControlTargetPtr step() override;

    //! Informs on what kind of control targets this control mode generates.
    virtual QList<ControlTargetType> supportedTargets() override;

private:
    //! The fish model.
    Model m_model;
};

#endif // CATS2_MODEL_BASED_HPP
