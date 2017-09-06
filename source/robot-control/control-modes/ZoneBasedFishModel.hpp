#ifndef CATS2_ZONE_BASED_FISH_MODEL_HPP
#define CATS2_ZONE_BASED_FISH_MODEL_HPP

#include "GenericFishModel.hpp"

/*!
 * Makes the robots to follow the zone-based fish model.
 */
class ZoneBasedFishModel : public GenericFishModel
{
    Q_OBJECT
public:
    //! Constructor.
    explicit ZoneBasedFishModel(FishBot* robot);
    //! Destructor.
    virtual ~ZoneBasedFishModel() override;

protected slots:
    //! Sets the model parameters from the settings.
    virtual void updateModelParameters() override { updateZoneBasedModelParameters(); }

protected:
    //! Initializes the model based on the setup map and parameters.
    virtual void resetModel() override { resetZoneBasedModel(); }

private:
    //! Initializes the model with walls.
    void resetZoneBasedModel();
    //! Sets the model parameters from the settings.
    void updateZoneBasedModelParameters();
};

#endif // CATS2_ZONE_BASED_FISH_MODEL_HPP
