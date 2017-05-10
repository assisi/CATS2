#include "ModelBased.hpp"
#include "FishBot.hpp"

#include "model/factory.hpp"
#include "model/model.hpp"

#include <QtCore/QDebug>
#include <QtCore/QtMath>

using namespace Fishmodel;

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
		// XXX HACK
        m_arena.reset(new Fishmodel::Arena("./dependencies/fish-model/arenas/SetupLargeModelzoneBlack70x70v2.png", size));
		std::cout << "@@@@ DEBUGMAPS1: " << size.first << " " << size.second << std::endl;
        Fishmodel::SimulationFactory factory(*m_arena);
        factory.nbFishes = RobotControlSettings::get().numberOfAnimals();
        factory.nbRobots = 1; // we generate one simulator for every robot
        factory.nbVirtuals = 0;
        factory.behaviorFishes = "ZoneDependantBM";
        factory.behaviorRobots = "ZoneDependantBM";
        factory.behaviorVirtuals = "ZoneDependantBM";
		factory.nbZones = 6;
        // create the simulator
        m_sim = factory.create();
        const FishModelSettings& fishModelSettings = RobotControlSettings::get().fishModelSettings();
        m_sim->dt = fishModelSettings.dt;
		std::cout << "@@@@ DEBUGMAPS2" << std::endl;

		std::vector<std::pair<Coord_t,Coord_t>> wallsCoord_ =
		{	{{0.015, 0.980}, {0.430, 0.980}},
			{{0.430, 0.980}, {0.430, 0.672}},
			{{0.430, 0.672}, {0.690, 0.390}},
			{{0.690, 0.390}, {0.980, 0.390}},
			{{0.980, 0.390}, {0.980, 0.010}},
			{{0.980, 0.010}, {0.580, 0.010}},
			{{0.580, 0.010}, {0.580, 0.010}},
			{{0.580, 0.314}, {0.315, 0.560}},
			{{0.315, 0.560}, {0.015, 0.560}},
			{{0.015, 0.560}, {0.015, 0.980}},
		};

		std::vector<std::pair<Coord_t,Coord_t>> wallsDirectionCoord_ =
		{	{{0.015, 0.980}, {0.430, 0.980}},
			{{0.430, 0.980}, {0.430, 0.672}},
			{{0.430, 0.672}, {0.690, 0.390}},
			{{0.690, 0.390}, {0.980, 0.390}},
			{{0.980, 0.390}, {0.980, 0.010}},
			{{0.980, 0.010}, {0.580, 0.010}},
			{{0.580, 0.010}, {0.580, 0.010}},
			{{0.580, 0.314}, {0.315, 0.560}},
			{{0.315, 0.560}, {0.015, 0.560}},
			{{0.015, 0.560}, {0.015, 0.980}},
		};

		// {'alpha': array([ 17.61884497,  49.11416896,  44.39342562,  42.73956526,  18.78914823]), 'kappaf': array([  1.71532289,  19.97715141,  22.47085689,   8.99530726,  29.42685055]), 'kappa0': array([  1.3003376 ,  16.22673487,  26.67371188,   9.84972902,  20.83194278]), 'gammaz': array([[ 0.88439782,  0.99330759,  0.07004122,  0.01663702,  0.08520822],       [ 0.10844928,  0.96854988,  0.07004122,  0.77765308,  0.08520822],       [ 0.14619926,  0.96854988,  0.32496295,  0.77765308,  0.99379137],       [ 0.9234174 ,  0.02888792,  0.32496295,  0.49465561,  0.99379137],       [ 0.99153215,  0.02888792,  0.115074  ,  0.49465561,  0.83885327]])}

        for(auto& a: m_sim->agents) {
			std::cout << "@@@@ DEBUGMAPS3" << std::endl;
			ZoneDependantBehavior* zdb = reinterpret_cast<ZoneDependantBehavior*>(a.second.get());
			std::cout << "@@@@ DEBUGMAPS3-1" << std::endl;

			ZonedBM* bm1 = reinterpret_cast<ZonedBM*>(zdb->behavior(1));
			ZonedBM* bm2 = reinterpret_cast<ZonedBM*>(zdb->behavior(2));
			ZonedBM* bm3 = reinterpret_cast<ZonedBM*>(zdb->behavior(3));
			ZonedBM* bm4 = reinterpret_cast<ZonedBM*>(zdb->behavior(4));
			ZonedBM* bm5 = reinterpret_cast<ZonedBM*>(zdb->behavior(5));
			std::cout << "@@@@ DEBUGMAPS3-2" << std::endl;

			bm1->kappaNeutCenter = 1.3003376;
			bm2->kappaNeutCenter = 16.22673487;
			bm3->kappaNeutCenter = 26.67371188;
			bm4->kappaNeutCenter = 9.84972902;
			bm5->kappaNeutCenter = 20.83194278;
			std::cout << "@@@@ DEBUGMAPS3-3" << std::endl;

			bm1->kappaFishes = 1.71532289;
			bm2->kappaFishes = 19.97715141;
			bm3->kappaFishes = 22.47085689;
			bm4->kappaFishes = 8.99530726;
			bm5->kappaFishes = 29.42685055;

			bm1->alphasCenter = 17.61884497;
			bm2->alphasCenter = 49.11416896;
			bm3->alphasCenter = 44.39342562;
			bm4->alphasCenter = 42.73956526;
			bm5->alphasCenter = 18.78914823;

			
			std::vector<real_t> affinity1 = {0.88439782,  0.99330759,  0.07004122,  0.01663702,  0.08520822};
			bm1->zonesAffinity(affinity1);
			std::vector<real_t> affinity2 = {0.10844928,  0.96854988,  0.07004122,  0.77765308,  0.08520822};
			bm2->zonesAffinity(affinity2);
			std::vector<real_t> affinity3 = {0.14619926,  0.96854988,  0.32496295,  0.77765308,  0.99379137};
			bm3->zonesAffinity(affinity3);
			std::vector<real_t> affinity4 = {0.9234174 ,  0.02888792,  0.32496295,  0.49465561,  0.99379137};
			bm4->zonesAffinity(affinity4);
			std::vector<real_t> affinity5 = {0.99153215,  0.02888792,  0.115074  ,  0.49465561,  0.83885327};
			bm5->zonesAffinity(affinity5);

			bm1->minSpeed = 0.0;
			bm2->minSpeed = 0.0;
			bm3->minSpeed = 0.0;
			bm4->minSpeed = 0.0;
			bm5->minSpeed = 0.0;

			bm1->maxSpeed = 0.20;
			bm2->maxSpeed = 0.20;
			bm3->maxSpeed = 0.20;
			bm4->maxSpeed = 0.20;
			bm5->maxSpeed = 0.20;

			// [array([ 0.0304615 ,  0.07993734,  0.10186769,  0.12054464,  0.14286059, 0.15312688,  0.14098084,  0.10748283,  0.07632245,  0.04641523]),
			// array([ 0.02276516,  0.09278528,  0.16815177,  0.19097442,  0.18189135, 0.13670595,  0.10106352,  0.05881   ,  0.03190572,  0.01494682]),
			// array([ 0.02834487,  0.10491745,  0.16764306,  0.18285106,  0.17604592, 0.14894372,  0.10225457,  0.05355346,  0.02585952,  0.00958637]),
			// array([ 0.04810033,  0.13980081,  0.17786795,  0.17989672,  0.15912947, 0.12113611,  0.08011804,  0.04758392,  0.02788639,  0.01848027]),
			// array([ 0.05988912,  0.14521016,  0.18034425,  0.1737687 ,  0.15442883, 0.11961707,  0.07813306,  0.04635121,  0.02688241,  0.01537519])]
			bm1->speedHistogram = {0.0304615 ,  0.07993734,  0.10186769,  0.12054464,  0.14286059, 0.15312688,  0.14098084,  0.10748283,  0.07632245,  0.04641523};
			bm2->speedHistogram = {0.02276516,  0.09278528,  0.16815177,  0.19097442,  0.18189135, 0.13670595,  0.10106352,  0.05881   ,  0.03190572,  0.01494682};
			bm3->speedHistogram = {0.02834487,  0.10491745,  0.16764306,  0.18285106,  0.17604592, 0.14894372,  0.10225457,  0.05355346,  0.02585952,  0.00958637};
			bm4->speedHistogram = {0.04810033,  0.13980081,  0.17786795,  0.17989672,  0.15912947, 0.12113611,  0.08011804,  0.04758392,  0.02788639,  0.01848027};
			bm5->speedHistogram = {0.05988912,  0.14521016,  0.18034425,  0.1737687 ,  0.15442883, 0.11961707,  0.07813306,  0.04635121,  0.02688241,  0.01537519};

			bm1->wallsCoord = wallsCoord_;
			bm2->wallsCoord = wallsCoord_;
			bm3->wallsCoord = wallsCoord_;
			bm4->wallsCoord = wallsCoord_;
			bm5->wallsCoord = wallsCoord_;

			bm1->wallsDirectionCoord = wallsDirectionCoord_;
			bm2->wallsDirectionCoord = wallsDirectionCoord_;
			bm3->wallsDirectionCoord = wallsDirectionCoord_;
			bm4->wallsDirectionCoord = wallsDirectionCoord_;
			bm5->wallsDirectionCoord = wallsDirectionCoord_;

			bm1->followWalls = true;
			bm2->followWalls = false;
			bm3->followWalls = false;
			bm4->followWalls = true;
			bm5->followWalls = true;
			std::cout << "@@@@ DEBUGMAPS4" << std::endl;

			bm1->reinit();
			bm2->reinit();
			bm3->reinit();
			bm4->reinit();
			bm5->reinit();
			std::cout << "@@@@ DEBUGMAPS5" << std::endl;
        }
			std::cout << "@@@@ DEBUGMAPS6" << std::endl;

        //// create the arena
        //m_arena.reset(new Fishmodel::Arena(m_currentGrid, size));
        //Fishmodel::SimulationFactory factory(*m_arena);
        //factory.nbFishes = RobotControlSettings::get().numberOfAnimals();
        //factory.nbRobots = 1; // we generate one simulator for every robot
        //factory.nbVirtuals = 0;
        //factory.behaviorFishes = "BM";
        //factory.behaviorRobots = "BM";
        //factory.behaviorVirtuals = "BM";
        //// create the simulator
        //m_sim = factory.create();
        //const FishModelSettings& fishModelSettings = RobotControlSettings::get().fishModelSettings();
        //m_sim->dt = fishModelSettings.dt;
        //for(auto& a: m_sim->agents) {
        //    a.first->length = fishModelSettings.length;
        //    a.first->width = fishModelSettings.width;
        //    a.first->height = fishModelSettings.height;
        //    a.first->fov = fishModelSettings.fov;
        //    a.first->meanSpeed = fishModelSettings.meanSpeed;
        //    a.first->varSpeed = fishModelSettings.varSpeed;
        //    a.first->maxTurningRate = M_PI_2;
        //    Fishmodel::BM* bm = reinterpret_cast<Fishmodel::BM*>(a.second.get());
        //    bm->kappaFishes = fishModelSettings.kappaFishes;
        //    bm->alphasCenter = fishModelSettings.alphasCenter;
        //    bm->kappaNeutCenter = fishModelSettings.kappaNeutCenter;
        //    bm->repulsionFromAgentsAtDist = fishModelSettings.repulsionFromAgentsAtDist;
        //}

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
