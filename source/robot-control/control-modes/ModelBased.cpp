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
    // updates the model parameters on change
    connect(&RobotControlSettings::get(),
            &RobotControlSettings::notifyFishModelSettingsChanged,
            this, &ModelBased::updateModelParameters);
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
    // in the beginning the model always check the position of fish
    m_parameters.ignoreFish = false; // TODO : better move this to GUI instead and do not reset it here
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
            status += QString(", dist. %1 m")
                    .arg(robotPosition.distance2dTo(m_targetPosition), 0, 'f', 3);
        }
        // TODO : temporary, to remove
        // check if the target position is inside the model area
        if (!containsPoint(m_targetPosition)) {
            qDebug() << "Attention: the target position is outside of the map";
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
//        cv::imshow( "ModelGrid", m_currentGrid);
    }
}

/*!
 * Sets the model parameters from the settings.
 */
void ModelBased::updateModelParameters()
{
    const FishModelSettings& fishModelSettings = RobotControlSettings::get().fishModelSettings();
    m_sim->dt = fishModelSettings.dt;
    for (auto& a: m_sim->agents) {
        a.first->length = fishModelSettings.length;
        a.first->width = fishModelSettings.width;
        a.first->height = fishModelSettings.height;
        a.first->fov = fishModelSettings.fov;
        a.first->meanSpeed = fishModelSettings.meanSpeed;
        a.first->varSpeed = fishModelSettings.varSpeed;
        a.first->maxTurningRate = M_PI_2;
        Fishmodel::BMWithWalls* bmww = reinterpret_cast<Fishmodel::BMWithWalls*>(a.second.get());
        bmww->kappaFishes = fishModelSettings.kappaFishes;
        bmww->alpha = fishModelSettings.alpha;
        bmww->kappaNeutCenter = fishModelSettings.kappaNeutCenter;
        bmww->repulsionFromAgentsAtDist = fishModelSettings.repulsionFromAgentsAtDist;
        bmww->wallDistanceThreshold = fishModelSettings.wallDistanceThreshold;
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

    size_t agentIndex = 0;
    if (!m_parameters.ignoreFish) {
        // update the fish positions in the model
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
    }
    size_t detectedAgentNum = agentIndex;
    for (agentIndex = detectedAgentNum;
         agentIndex < RobotControlSettings::get().numberOfAnimals();
         ++agentIndex)
    {
        m_sim->fishes[agentIndex].first->present = false;
    }

    // update position of the robot in model
    if (!m_parameters.ignoreRobot) {
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
    } else {
        m_sim->robots[0].first->present = false;
    }

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
        qDebug() << QString("Model parameters are updated, ignore-fish:%1,"
                            "ignore-robot:%2")
                    .arg(m_parameters.ignoreFish)
                    .arg(m_parameters.ignoreRobot);
        resetModel();
    }
}
