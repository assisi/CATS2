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
    m_sim(nullptr),
    m_parameters(),
    m_targetPosition(PositionMeters::invalidPosition()),
    m_targetUpdateTimer()
{
    resetModel();
//    cv::namedWindow("ModelGrid", cv::WINDOW_NORMAL);
}

/*!
 * Destructor.
 */
ModelBased::~ModelBased()
{
//    cv::destroyWindow("ModelGrid");
    qDebug() << "Destroying the object";
}

/*!
 * Called when the control mode is activated. Used to reset mode's parameters.
 */
void ModelBased::start()
{
    m_targetUpdateTimer.reset();
    // compute the first target position
    m_targetPosition = computeTargetPosition();
}

/*!
 * The step of the control mode.
 */
ControlTargetPtr ModelBased::step()
{
    // if it's the time to update the model
    if (m_sim && m_targetUpdateTimer.isTimedOutSec(m_sim->dt)) {
        m_targetPosition = computeTargetPosition();
        m_targetUpdateTimer.reset();
    }

    if (m_targetPosition.isValid()) {
        PositionMeters robotPosition = m_robot->state().position();
        QString status;
        if (m_parameters.ignoreFish)
            status = "ignore fish";
        else
            status = "follow fish";
        if (robotPosition.isValid()) {
            status += QString(", target distance %1 m")
                    .arg(robotPosition.distance2DTo(m_targetPosition), 0, 'f', 3);
        }
        emit notifyControlModeStatus(status);
        return ControlTargetPtr(new TargetPosition(m_targetPosition));
    }
    // otherwise the robot doesn't move
    return ControlTargetPtr(new TargetSpeed(0, 0));
}

/*!
 * Informs on what kind of control targets this control mode generates.
 */
QList<ControlTargetType> ModelBased::supportedTargets()
{
    return QList<ControlTargetType>({ControlTargetType::SPEED,
                                     ControlTargetType::POSITION});
}

/*!
 * Initializes the model based on the setup map and parameters.
 */
void ModelBased::resetModel()
{
    if (!m_currentGrid.empty()) {
        // size of the area covered by the matrix
        Fishmodel::Coord_t size = {m_currentGrid.cols * m_gridSizeMeters,
                                   m_currentGrid.rows * m_gridSizeMeters};
        // create the arena
        m_arena.reset(new Fishmodel::Arena(m_currentGrid, size));
        Fishmodel::SimulationFactory factory(*m_arena);
        if (!m_parameters.ignoreFish)
            factory.nbFishes = RobotControlSettings::get().numberOfAnimals();
        else
            factory.nbFishes = 0;
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
//        cv::imshow( "ModelGrid", m_currentGrid);
    }
}

/*!
 * Computes the target position from the model.
 */
PositionMeters ModelBased::computeTargetPosition()
{
    if ((m_sim == nullptr) || (m_sim && (m_sim->fishes.size() == 0))) {
        return PositionMeters::invalidPosition();
    }

//    cv::imshow( "ModelGrid", m_currentGrid);

//    // if no data is available, don't update the target
//    if (m_robot->fishStates().size() == 0) {
//        qDebug() << "No fish detected, impossible to run the model";
//        // returning the previous target
//        return m_targetPosition;
//    }

    // set the target invalid until it's computed
    PositionMeters targetPosition;
    targetPosition.setValid(false);

    if (!m_parameters.ignoreFish) {
        // update the fish positions in the model
        size_t agentIndex = 0;
        for (StateWorld& state : m_robot->fishStates()){
            if (agentIndex < m_sim->fishes.size()) {
                if (state.position().isValid() && containsPoint(state.position())) {
                    // the positions are normalized to fit the matrix
                    m_sim->fishes[agentIndex].first->headPos.first = state.position().x() - minX();
                    m_sim->fishes[agentIndex].first->headPos.second = state.position().y() - minY();

                    if (state.orientation().isValid())
                        m_sim->fishes[agentIndex].first->direction =
                                state.orientation().angleRad();
                    else
                        m_sim->fishes[agentIndex].first->direction = 0;

                    m_sim->fishes[agentIndex].first->present = true;
                    agentIndex++;
                }
            } else {
                qDebug() << "Number of fish in the simulator is wrongly initialized.";
                break;
            }
        }
        size_t detectedAgentNum = agentIndex;
        for (agentIndex = detectedAgentNum;
             agentIndex < RobotControlSettings::get().numberOfAnimals();
             ++agentIndex)
        {
            m_sim->fishes[agentIndex].first->present = false;
        }
    }

    // update position of the robot in model
    PositionMeters robotPosition = m_robot->state().position();
    OrientationRad robotOrientation = m_robot->state().orientation();
    if (robotPosition.isValid() && containsPoint(robotPosition) &&
        (m_sim->robots.size() == 1))
    {
        m_sim->robots[0].first->headPos.first = robotPosition.x() - minX();
        m_sim->robots[0].first->headPos.second = robotPosition.y() - minY();

        if (robotOrientation.isValid()) {
            m_sim->robots[0].first->direction = robotOrientation.angleRad();
        } else {
            m_sim->robots[0].first->direction = 0;
        }
        m_sim->robots[0].first->present = true;
    } else {
        qDebug() << "The robot position is outside of the setup "
                                   "area or invalid";
        m_sim->robots[0].first->present = false;
    }

    // FIXME : what to do with this?
    //		// XXX dirty
    //		BM* behav = (BM*)sim->robots[i].second;
    //		//behav->alphasCenter = 100. * 10. / (double)(detected + settings.numberOfCASUS);
    //		behav->alphasCenter = 5000. / (double)(detected + settings.numberOfCASUS);
    //	}

    // run the simulation
    m_sim->step();
    // get the target value
    if (m_sim->robots.size() > 0) { // we have only one robot so it is #0
        targetPosition.setX((m_sim->robots[0].first->headPos.first +
                            m_sim->robots[0].first->tailPos.first) / 2. + minX());
        targetPosition.setY((m_sim->robots[0].first->headPos.second +
                            m_sim->robots[0].first->tailPos.second) / 2. + minY());
        targetPosition.setValid(true);
//        qDebug() << QString("New target is %1")
//                    .arg(targetPosition.toString());
    }

    return targetPosition;
}

/*!
 * Sets the model's parameters. Every time the parameters are changed, the
 * model is reset.
 */
void ModelBased::setParameters(ModelParameters parameters)
{
    if (parameters != m_parameters) {
        m_parameters = parameters;
        resetModel();
    }
}
