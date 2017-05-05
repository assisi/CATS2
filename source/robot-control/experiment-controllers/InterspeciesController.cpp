#include "InterspeciesController.hpp"

#include "FishBot.hpp"

/*!
 * Constructor.
 */
InterspeciesController::InterspeciesController(FishBot* robot) :
    ExperimentController(robot, ExperimentControllerType::INTERSPECIES)
{

}

/*!
 * Returns the control values for given position.
 */
ExperimentController::ControlData InterspeciesController::step()
{
    ControlData controlData;

    // TODO : do stuff

    return controlData;
}

/*!
 * Called when the controller is activated. Used to reset parameters.
 */
void InterspeciesController::start()
{
    // TODO : do stuff
}


