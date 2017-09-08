#ifndef CATS2_MODEL_BASED_HPP
#define CATS2_MODEL_BASED_HPP

#include "GenericFishModel.hpp"

/*!
 * Makes the robots to follow the basic fish mode. Inherits GenericFishModel,
 * the name of the class is historic.
 */
class ModelBased : public GenericFishModel
{
    Q_OBJECT
public:
    //! Constructor.
    explicit ModelBased(FishBot* robot);
    //! Destructor.
    virtual ~ModelBased() override;

private slots:
    //! Sets the model parameters from the settings.
    virtual void updateModelParameters() override { updateBasicModelParameters(); }

protected:
    //! Initializes the model based on the setup map and parameters.
    virtual void resetModel() override { resetBasicModel(); }

private:
    //! Initializes the basic model.
    void resetBasicModel();
    //! Sets the model parameters from the settings.
    void updateBasicModelParameters();
};

#endif // CATS2_MODEL_BASED_HPP
