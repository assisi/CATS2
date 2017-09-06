#include "ModelBased.hpp"
#include "FishBot.hpp"

#include "model/factory.hpp"
#include "model/model.hpp"

#include <QtCore/QDebug>
#include <QtCore/QtMath>

/*!
 * Constructor.
 */
ModelBased::ModelBased(FishBot* robot) :
    GenericFishModel(robot, ControlModeType::FISH_MODEL)
{
    resetBasicModel();
}

/*!
 * Destructor.
 */
ModelBased::~ModelBased()
{
    qDebug() << "Destroying the object";
}

/*!
 * Initializes the model based on the setup map and parameters.
 */
void ModelBased::resetBasicModel()
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
        factory.behaviorFishes = "BM";
        factory.behaviorRobots = "BM";
        factory.behaviorVirtuals = "BM";
        // create the simulator
        m_sim = factory.create();
        updateBasicModelParameters();
//        cv::imshow( "ModelGrid", m_currentGrid);
    }
}

/*!
 * Sets the model parameters from the settings.
 */
void ModelBased::updateBasicModelParameters()
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
        Fishmodel::BM* bm = reinterpret_cast<Fishmodel::BM*>(a.second.get());
        bm->kappaFishes = fishModelSettings.basicFishModelSettings.kappaFishes;
        bm->alphasCenter = fishModelSettings.basicFishModelSettings.alphasCenter;
        bm->kappaNeutCenter = fishModelSettings.basicFishModelSettings.kappaNeutCenter;
        bm->repulsionFromAgentsAtDist = fishModelSettings.basicFishModelSettings.repulsionFromAgentsAtDist;
    }
}
