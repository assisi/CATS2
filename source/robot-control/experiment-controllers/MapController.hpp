#ifndef CATS2_MAP_CONTROLLER_HPP
#define CATS2_MAP_CONTROLLER_HPP

#include "ExperimentController.hpp"
#include "settings/MapControllerSettings.hpp"

#include <AgentState.hpp>

#include <QtCore/QString>

/*!
 * The control map for every given position of the experimental setup provides
 * the control mode and motion pattern to excecute in this position. Every robot
 * has it's own control map that it consults on every step.
 */
class MapController : public ExperimentController
{
    Q_OBJECT
public:
    //! Constructor. Gets robot and the settings
    MapController(FishBot* robot,
                  ExperimentControllerSettingsPtr settings);

public:
    //! Returns the control values for given position.
    virtual ControlData step() override;

private:
    //! The settings for this controller.
    MapControllerSettingsData m_settings;
};

#endif // CATS2_MAP_CONTROLLER_HPP
