#include "FishModelWithWalls.hpp"
#include "FishBot.hpp"

#include "model/factory.hpp"
#include "model/model.hpp"
#include "model/bmWithWalls.hpp"

#include <QtCore/QDebug>
#include <QtCore/QtMath>

/*!
 * Constructor.
 */
FishModelWithWalls::FishModelWithWalls(FishBot* robot) :
    GenericFishModel(robot, ControlModeType::FISH_MODEL_WITH_WALLS)
{
    resetModel();
}

/*!
 * Destructor.
 */
FishModelWithWalls::~FishModelWithWalls()
{
    qDebug() << "Destroying the object";
}

/*!
 * Initializes the model based on the setup map and parameters.
 */
void FishModelWithWalls::resetModel()
{
    if (!m_currentGrid.empty()) {
        // size of the area covered by the matrix
        Fishmodel::Coord_t size = {m_currentGrid.cols * m_gridSizeMeters,
                                   m_currentGrid.rows * m_gridSizeMeters};
        // create the arena
        m_arena.reset(new Fishmodel::Arena(m_currentGrid, size));
        Fishmodel::SimulationFactory factory(*m_arena);
        factory.nbFishes = RobotControlSettings::get().numberOfAnimals();
        factory.nbRobots = 1; // we generate one simulator for every robot
        factory.nbVirtuals = 0;
        factory.behaviorFishes = "BMWithWalls";
        factory.behaviorRobots = "BMWithWalls";
        factory.behaviorVirtuals = "BMWithWalls";
        factory.wallsCoord = setupWalls();
        // create the simulator
        m_sim = factory.create();
        updateModelParameters();
    }
}

/*!
 * Sets the model parameters from the settings.
 */
void FishModelWithWalls::updateModelParameters()
{
    const FishModelSettings& fishModelSettings = RobotControlSettings::get().fishModelSettings();
    m_sim->dt = fishModelSettings.agentParameters.dt;
    for (auto& a: m_sim->agents) {
        a.first->length = fishModelSettings.agentParameters.length;
        a.first->width = fishModelSettings.agentParameters.width;
        a.first->height = fishModelSettings.agentParameters.height;
        a.first->fov = fishModelSettings.agentParameters.fov;
        a.first->meanSpeed = fishModelSettings.agentParameters.meanSpeed;
        a.first->varSpeed = fishModelSettings.agentParameters.varSpeed;
        a.first->maxTurningRate = M_PI_2;
        Fishmodel::BMWithWalls* bmww = reinterpret_cast<Fishmodel::BMWithWalls*>(a.second.get());
        bmww->kappaFishes = fishModelSettings.fishModelWithWallsSettings.kappaFishes;
        bmww->alpha = fishModelSettings.fishModelWithWallsSettings.alpha;
        bmww->kappaNeutCenter = fishModelSettings.fishModelWithWallsSettings.kappaNeutCenter;
        bmww->repulsionFromAgentsAtDist = fishModelSettings.fishModelWithWallsSettings.repulsionFromAgentsAtDist;
        bmww->wallDistanceThreshold = fishModelSettings.fishModelWithWallsSettings.wallDistanceThreshold;
    }
}
