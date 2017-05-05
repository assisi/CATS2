#ifndef CATS2_INTERSPECIES_CONTROLLER_HPP
#define CATS2_INTERSPECIES_CONTROLLER_HPP

#include "ExperimentController.hpp"

/*!
 * The controller that implements the interspecies experiment. Designed for the
 * circular setup. The goal of the robot is (1) if in the master setup mode to
 * go in the selected direction (clock-wise or contre-clock-wise) in order to
 * influence the fish to go in the same direction (2) when in a slave setup mode
 * the goal is to go in the direction defined by the remote setup.
 */
class InterspeciesController : public ExperimentController
{
public:
    //! Constructor.
    InterspeciesController(FishBot* robot);

public:
    //! Called when the controller is activated. Used to reset parameters.
    virtual void start() override;
    //! Returns the control values for given position.
    virtual ControlData step() override;
};

#endif // CATS2_INTERSPECIES_CONTROLLER_HPP
