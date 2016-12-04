#ifndef CATS2_MODEL_BASED_HPP
#define CATS2_MODEL_BASED_HPP

#include "ControlMode.hpp"
#include "SetupMap.hpp"
#include "model/bm.hpp"

#include <AgentState.hpp>

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
    //! Initializes the model based on the setup map.
    void initModel();

    //! Computes the target position from the model.
    PositionMeters computeTargetPosition();

private:
    //! The map of the setup, used in path planning and modelling.
    SetupMap m_setupMap;

    //! The resolution of the setup matrix.
    static constexpr double ModelResolutionM = 0.005; // i.e. 5 mm

    //! The model related data.
    std::unique_ptr<Fishmodel::Arena> arena;
    std::unique_ptr<Fishmodel::Simulation> sim;
};

#endif // CATS2_MODEL_BASED_HPP
