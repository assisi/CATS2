#include "ModelBased.hpp"
#include "FishBot.hpp"

#include "model/factory.hpp"
#include "model/model.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
ModelBased::ModelBased(FishBot* robot) :
    ControlMode(robot, ControlModeType::MODEL_BASED),
    m_setupMap(RobotControlSettings::get().setupMap()),
    arena(nullptr),
    sim(nullptr)
{

}

/*!
 * The step of the control mode.
 */
ControlTargetPtr ModelBased::step()
{
    // if we track any fish
    if (m_robot->fishStates().size() > 0) {
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
        // build the matrix
        int cols = floor((m_setupMap.maxX() - m_setupMap.minX()) / ModelResolutionM + 0.5);
        int rows = floor((m_setupMap.maxY() - m_setupMap.minY()) / ModelResolutionM + 0.5);
        if ((cols > 0) && (rows > 0)) {
            // a matrix representing the setup
            cv::Mat arenaMatrix(rows, cols, CV_8U);
            // size of the area covered by the matrix
            Fishmodel::Coord_t size = {m_setupMap.maxX() - m_setupMap.minX(),
                                       m_setupMap.maxY() - m_setupMap.minY()};
            // fill the matrix
            double y = m_setupMap.minY();
            for (int row = 0; row < rows; ++row) { // rows go from min_y up to max_y
                double x = m_setupMap.minX();
                for (int col = 0; col < cols; ++col) { // cols go from min_x right to max_x
                    if (m_setupMap.containsPoint(PositionMeters(x, y)))
                        arenaMatrix.at<uchar>(row, col) = 1;
                    else
                        arenaMatrix.at<uchar>(row, col) = 0;
                    x += ModelResolutionM;
                }
                y += ModelResolutionM;
            }
            // create the arena
            arena.reset(new Fishmodel::Arena(arenaMatrix, size));
            Fishmodel::SimulationFactory factory(*arena);
            factory.nbFishes = RobotControlSettings::get().numberOfAnimals();
            factory.nbRobots = 1; // we generate one simulator for every robot
            factory.nbVirtuals = 0;
            factory.behaviorFishes = "NoBehavior";
            factory.behaviorRobots = "BM";
            factory.behaviorVirtuals = "BM";
            // create the simulator
            sim = factory.create();
            const FishModelSettings& fishModelSettings = RobotControlSettings::get().fishModelSettings();
            sim->dt = fishModelSettings.dt;
            for(auto& a: sim->agents) {
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
        if (state.position().isValid() && m_setupMap.containsPoint(state.position())) {
            sim->fishes[agentIndex].first->headPos.first = state.position().x() - m_setupMap.minX(); // NOTE : the positions are normalized to fit the matrix
            sim->fishes[agentIndex].first->headPos.second = state.position().y() - m_setupMap.minY();
            if (state.orientation().isValid())
                sim->fishes[agentIndex].first->direction = state.orientation().angleRad();
            else
                sim->fishes[agentIndex].first->direction = 0;
            sim->fishes[agentIndex].first->present = true;
            agentIndex++;
        }
    }
    size_t detectedAgentNum = agentIndex;
    for (agentIndex = detectedAgentNum; agentIndex < RobotControlSettings::get().numberOfAnimals(); ++agentIndex) {
        sim->fishes[agentIndex].first->present = false;
    }

    // TODO : to check if we need it
//    // Update position of robots in model
//    for(int i = 0; i < settings.numberOfCASUS; ++i) {
//		if(settings.robots[i].useRobotPoseInModel) {
//			sim->robots[i].first->headPos.first = static_cast<double>(FishCASUS[i].Pos.at<float>(0)) / static_cast<double>(imgWidth);
//			sim->robots[i].first->headPos.second = static_cast<double>(FishCASUS[i].Pos.at<float>(1)) / static_cast<double>(imgHeight);
//			sim->robots[i].first->direction = FishCASUS[i].Pos.at<float>(2);
//		}
//		sim->robots[i].first->present = true;

//		// XXX dirty
//		BM* behav = (BM*)sim->robots[i].second;
//		//behav->alphasCenter = 100. * 10. / (double)(detected + settings.numberOfCASUS);
//		behav->alphasCenter = 5000. / (double)(detected + settings.numberOfCASUS);
//	}

    if ((m_robot->fishStates().size() > 0 ) && (detectedAgentNum > 0) && sim)  {
        sim->step();
        // get the target value
        if (sim->robots.size() > 0) { // we have only one robot so it is #0
            targetPosition.setX((sim->robots[0].first->headPos.first + sim->robots[0].first->tailPos.first) / 2. + m_setupMap.minX());
            targetPosition.setY((sim->robots[0].first->headPos.second + sim->robots[0].first->tailPos.second) / 2. + m_setupMap.minY());
            targetPosition.setValid(true);
        }
    }
    return targetPosition;
}
