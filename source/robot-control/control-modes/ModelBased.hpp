#ifndef CATS2_MODEL_BASED_HPP
#define CATS2_MODEL_BASED_HPP

#include "ControlMode.hpp"
#include <AgentState.hpp>

#include "model/bm.hpp"

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
    //! The fish model (BM = Bertrand's model).
    Fishmodel::BM m_model;
};

#endif // CATS2_MODEL_BASED_HPP
