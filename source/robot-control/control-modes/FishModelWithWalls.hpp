#ifndef CATS2_FISH_MODEL_WITH_WALLS_HPP
#define CATS2_FISH_MODEL_WITH_WALLS_HPP

#include "GenericFishModel.hpp"

/*!
 * Makes the robots to follow the fish model with walls.
 */
class FishModelWithWalls : public GenericFishModel
{
    Q_OBJECT
public:
    //! Constructor.
    explicit FishModelWithWalls(FishBot* robot);
    //! Destructor.
    virtual ~FishModelWithWalls() override;

protected slots:
    //! Sets the model parameters from the settings.
    virtual void updateModelParameters() override { updateFishModelWithWallsParameters(); }

protected:
    //! Initializes the model based on the setup map and parameters.
    virtual void resetModel() override { resetModelWithWalls(); }

private:
    //! Initializes the model with walls.
    void resetModelWithWalls();
    //! Sets the model parameters from the settings.
    void updateFishModelWithWallsParameters();
};

#endif // CATS2_FISH_MODEL_WITH_WALLS_HPP
