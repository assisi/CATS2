#ifndef CATS2_MAP_CONTROLLER_HPP
#define CATS2_MAP_CONTROLLER_HPP

#include "ExperimentController.hpp"

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
    //! Constructor. Gets the file name containing the control map description.
    MapController(FishBot* robot, QString controlMapFileName);

public:
    //! Returns the control values for given position.
    virtual ControlData step() override;
};

#endif // CATS2_MAP_CONTROLLER_HPP
