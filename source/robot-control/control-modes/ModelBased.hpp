#ifndef CATS2_MODEL_BASED_HPP
#define CATS2_MODEL_BASED_HPP

#include "ControlMode.hpp"
#include "SetupMap.hpp"
#include "navigation/GridBasedMethod.hpp"
#include "model/bm.hpp"

#include <AgentState.hpp>

/*!
 * Makes the robots to follow the fish mode.
 */
class ModelBased : public ControlMode, public GridBasedMethod
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
    //! Initializes the model based on the setup map.
    void initModel();

    //! Computes the target position from the model.
    PositionMeters computeTargetPosition();

private:
    //! The resolution of the setup matrix.
    static constexpr double ModelResolutionM = 0.005; // i.e. 5 mm

    //! The model related data.
    std::unique_ptr<Fishmodel::Arena> m_arena;
    std::unique_ptr<Fishmodel::Simulation> m_sim;
};

#endif // CATS2_MODEL_BASED_HPP
