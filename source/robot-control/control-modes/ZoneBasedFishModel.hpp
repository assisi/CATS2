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

    //! Gives the setup's max-x value statistics id.
    QString maxXStatisticsId() const;
    //! Gives the setup's max-y value statistics id.
    QString maxYStatisticsId() const;
    //! Gives the setup's min-x value statistics id.
    QString minXStatisticsId() const;
    //! Gives the setup's min-y value statistics id.
    QString minYStatisticsId() const;
};

#endif // CATS2_ZONE_BASED_FISH_MODEL_HPP
