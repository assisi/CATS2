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
    ControlMode(robot, ControlModeType::MODEL_BASED),
    GridBasedMethod(ModelResolutionM),
    m_arena(nullptr),
    m_sim(nullptr)
{
    initModel();
}

/*!
 * The step of the control mode.
 */
ControlTargetPtr ModelBased::step()
{
    // if we track any fish
    if ((m_robot->fishStates().size() > 0) &&
            m_sim &&
            (m_sim->fishes.size() > 0)) {
        PositionMeters targetPosition = computeTargetPosition();
        if (targetPosition.isValid()) {
            return ControlTargetPtr(new TargetPosition(targetPosition));
        }
    }
    // otherwise the robot doesn't move
    return ControlTargetPtr(new TargetSpeed(0, 0));
}

/*!
 * Informs on what kind of control targets this control mode generates.
 */
QList<ControlTargetType> ModelBased::supportedTargets()
{
    return QList<ControlTargetType>({ControlTargetType::POSITION});
}

/*!
 * Initializes the model based on the setup map.
 */
void ModelBased::initModel()
{
    if (m_setupMap.isValid()) {
        // build the matrix representing the setup map
        // it's a binary matrix, with 1 corresponding to the free space and 0 to
        // occupied area
        cv::Mat arenaMatrix = generateGrid();
        if ((arenaMatrix.rows > 0) && (arenaMatrix.cols > 0))  {
            // size of the area covered by the matrix
            Fishmodel::Coord_t size = {arenaMatrix.cols * m_gridSizeMeters,
                                       arenaMatrix.rows * m_gridSizeMeters};
            // create the arena
            m_arena.reset(new Fishmodel::Arena(arenaMatrix, size));
            Fishmodel::SimulationFactory factory(*m_arena);
            factory.nbFishes = RobotControlSettings::get().numberOfAnimals();
            factory.nbRobots = 1; // we generate one simulator for every robot
            factory.nbVirtuals = 0;
            factory.behaviorFishes = "BM";
            factory.behaviorRobots = "BM";
            factory.behaviorVirtuals = "BM";
            // create the simulator
            m_sim = factory.create();
            const FishModelSettings& fishModelSettings = RobotControlSettings::get().fishModelSettings();
            m_sim->dt = fishModelSettings.dt;
            for(auto& a: m_sim->agents) {
                a.first->length = fishModelSettings.length;
                a.first->width = fishModelSettings.width;
                a.first->height = fishModelSettings.height;
                a.first->fov = fishModelSettings.fov;
                a.first->meanSpeed = fishModelSettings.meanSpeed;
                a.first->varSpeed = fishModelSettings.varSpeed;
                a.first->maxTurningRate = M_PI_2;
                Fishmodel::BM* bm = reinterpret_cast<Fishmodel::BM*>(a.second.get());
                bm->kappaFishes = fishModelSettings.kappaFishes;
                bm->alphasCenter = fishModelSettings.alphasCenter;
                bm->kappaNeutCenter = fishModelSettings.kappaNeutCenter;
                bm->repulsionFromAgentsAtDist = fishModelSettings.repulsionFromAgentsAtDist;
            }
        }
    }
}

/*!
 * Computes the target position from the model.
 */
PositionMeters ModelBased::computeTargetPosition()
{
    // set the target invalid until it's computed
    PositionMeters targetPosition;
    targetPosition.setValid(false);

    // update the fish positions in the model
    size_t agentIndex = 0;
    for (StateWorld& state : m_robot->fishStates()){
        if (agentIndex < m_sim->fishes.size()) {
            if (state.position().isValid() &&
                    m_setupMap.containsPoint(state.position())) {
                // NOTE : the positions are normalized to fit the matrix, they
                // are compared with the 0 as the grid is slightly shifted with
                // respect to the setup min borders
                m_sim->fishes[agentIndex].first->headPos.first =
                        qMax(state.position().x() - minX(), 0.);
                m_sim->fishes[agentIndex].first->headPos.second =
                        qMax(state.position().y() - minY(), 0.);

                if (state.orientation().isValid())
                    m_sim->fishes[agentIndex].first->direction =
                            state.orientation().angleRad();
                else
                    m_sim->fishes[agentIndex].first->direction = 0;

                m_sim->fishes[agentIndex].first->present = true;
                agentIndex++;
            }
        } else {
            qDebug() << Q_FUNC_INFO
                << "Number of fish in the simulator is wrongly initialized.";
            break;
        }
    }
    size_t detectedAgentNum = agentIndex;
    for (agentIndex = detectedAgentNum; agentIndex < RobotControlSettings::get().numberOfAnimals(); ++agentIndex) {
        m_sim->fishes[agentIndex].first->present = false;
    }

    // update position of the robot in model
    PositionMeters robotPosition = m_robot->state().position();
    OrientationRad robotOrientation = m_robot->state().orientation();
    if (robotPosition.isValid() &&
            m_setupMap.containsPoint(robotPosition) &&
            (m_sim->robots.size() == 1)) {
        m_sim->robots[0].first->headPos.first =
                qMax(robotPosition.x() - minX(), 0.);
        m_sim->robots[0].first->headPos.second =
                qMax(robotPosition.y() - minY(), 0.);

        if (robotOrientation.isValid()) {
            m_sim->robots[0].first->direction =
                    robotOrientation.angleRad();
        } else {
            m_sim->robots[0].first->direction = 0;
        }

        m_sim->robots[0].first->present = true;
    }

    // FIXME : what to do with this?
//		// XXX dirty
//		BM* behav = (BM*)sim->robots[i].second;
//		//behav->alphasCenter = 100. * 10. / (double)(detected + settings.numberOfCASUS);
//		behav->alphasCenter = 5000. / (double)(detected + settings.numberOfCASUS);
//	}

    if ((m_robot->fishStates().size() > 0 ) && (detectedAgentNum > 0))  {
        m_sim->step();
        // get the target value
        if (m_sim->robots.size() > 0) { // we have only one robot so it is #0
            targetPosition.setX((m_sim->robots[0].first->headPos.first +
                                m_sim->robots[0].first->tailPos.first) / 2. + minX());
            targetPosition.setY((m_sim->robots[0].first->headPos.second +
                                m_sim->robots[0].first->tailPos.second) / 2. + minY());
            targetPosition.setValid(true);
            qDebug() << Q_FUNC_INFO  << QString("New target is %1")
                        .arg(targetPosition.toString());
        }
    }
    return targetPosition;
}
