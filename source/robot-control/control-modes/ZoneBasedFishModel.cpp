#include "ZoneBasedFishModel.hpp"
#include "FishBot.hpp"

#include "model/factory.hpp"
#include "model/model.hpp"
#include "model/bmWithWalls.hpp"

#include "statistics/StatisticsPublisher.hpp"

#include <QtCore/QDebug>
#include <QtCore/QtMath>

/*!
 * Constructor.
 */
ZoneBasedFishModel::ZoneBasedFishModel(FishBot* robot) :
    GenericFishModel(robot, ControlModeType::ZONE_BASED_FISH_MODEL)
{
    resetZoneBasedModel();
    // register statistics
    StatisticsPublisher::get().addStatistics(maxXStatisticsId());
    StatisticsPublisher::get().addStatistics(maxYStatisticsId());
    StatisticsPublisher::get().addStatistics(minXStatisticsId());
    StatisticsPublisher::get().addStatistics(minYStatisticsId());

    // update statistics once as they don't change
    StatisticsPublisher::get().updateStatistics(maxXStatisticsId(), maxX());
    StatisticsPublisher::get().updateStatistics(maxYStatisticsId(), maxY());
    StatisticsPublisher::get().updateStatistics(minXStatisticsId(), minX());
    StatisticsPublisher::get().updateStatistics(minYStatisticsId(), minY());
}

/*!
 * Destructor.
 */
ZoneBasedFishModel::~ZoneBasedFishModel()
{
    qDebug() << "Destroying the object";
}

/*!
 * Initializes the model based on the setup map and parameters.
 */
void ZoneBasedFishModel::resetZoneBasedModel()
{
    if (!m_currentGrid.empty()) {
        // as we use a multi-zone model a new grid matrix need to be constructed
        cv::Mat zoneBasedGrid;
        m_currentGrid.copyTo(zoneBasedGrid);
        // size of the area covered by the matrix
        Fishmodel::Coord_t size = {zoneBasedGrid.cols * m_gridSizeMeters,
                                   zoneBasedGrid.rows * m_gridSizeMeters};
        // reset the grid values
        const FishModelSettings& fishModelSettings = RobotControlSettings::get().fishModelSettings();
        if (fishModelSettings.zonedFishModelSettings.size() == 0) {
            m_sim = nullptr;
            return;
        }
        for (int col = 0; col < zoneBasedGrid.cols; ++col)
            for (int row = 0; row < zoneBasedGrid.cols; ++row) {
                QPoint point(col, row);
                PositionMeters position = gridNodeToPosition(point);
                bool foundZone = false;
                for (int zoneIndex = 0;
                     zoneIndex < fishModelSettings.zonedFishModelSettings.size();
                     ++zoneIndex)
                {
                    const ZonedFishModelSettings& settings = fishModelSettings.zonedFishModelSettings.at(zoneIndex);
                    // set the value for the zone that corresponds to this position
                    if (settings.contains(position)) {
                        zoneBasedGrid.at<uchar>(row, col) = (zoneIndex + 1) * 10;
                        foundZone = true;
                        break;
                    }
                }
                if (!foundZone)
                    zoneBasedGrid.at<uchar>(row, col) = 0;
            }

        // create the arena
        m_arena.reset(new Fishmodel::Arena(zoneBasedGrid, size));
        Fishmodel::SimulationFactory factory(*m_arena);
        factory.nbFishes = RobotControlSettings::get().numberOfAnimals();
        factory.nbRobots = 1; // we generate one simulator for every robot
        factory.nbVirtuals = 0;
        factory.behaviorFishes = "ZoneDependantBM";
        factory.behaviorRobots = "ZoneDependantBM";
        factory.behaviorVirtuals = "ZoneDependantBM";
        factory.nbZones = fishModelSettings.zonedFishModelSettings.size();
        // create the simulator
        m_sim = factory.create();
        updateZoneBasedModelParameters();
    }
}

/*!
 * Sets the model parameters from the settings.
 */
void ZoneBasedFishModel::updateZoneBasedModelParameters()
{
    const FishModelSettings& fishModelSettings = RobotControlSettings::get().fishModelSettings();
    m_sim->dt = fishModelSettings.agentParameters.dt;
    std::vector<GridBasedMethod::Edge> wallsCoordinates = setupWalls();

    for (auto& a: m_sim->agents) {
        a.first->length = fishModelSettings.agentParameters.length;
        a.first->width = fishModelSettings.agentParameters.width;
        a.first->height = fishModelSettings.agentParameters.height;
        a.first->fov = fishModelSettings.agentParameters.fov;
        a.first->meanSpeed = fishModelSettings.agentParameters.meanSpeed;
        a.first->varSpeed = fishModelSettings.agentParameters.varSpeed;
        a.first->maxTurningRate = M_PI_2;

        Fishmodel::ZoneDependantBehavior* zdb = reinterpret_cast<Fishmodel::ZoneDependantBehavior*>(a.second.get());
        for (int zoneIndex = 0;
             zoneIndex < fishModelSettings.zonedFishModelSettings.size();
             ++zoneIndex)
        {
            Fishmodel::ZonedBM* bm = reinterpret_cast<Fishmodel::ZonedBM*>(zdb->behavior(zoneIndex + 1));
            const ZonedFishModelSettings& settings = fishModelSettings.zonedFishModelSettings.at(zoneIndex);
            bm->kappaNeutCenter = settings.kappaNeutCenter;
            bm->kappaFishes = settings.kappaFishes;
            bm->alphasCenter = settings.alphasCenter;
            bm->zonesAffinity(const_cast<ZonedFishModelSettings&>(settings).zonesAffinity);
            bm->minSpeed = settings.minSpeed;
            bm->maxSpeed = settings.maxSpeed;
            bm->speedHistogram = settings.speedHistogram;
            bm->wallsCoord = wallsCoordinates;
            bm->wallsDirectionCoord = wallsCoordinates;
            bm->followWalls = settings.followWalls;
            bm->reinit();
        }
    }
}

/*!
 * Gives the setup's max-x value statistics id.
 */
QString ZoneBasedFishModel::maxXStatisticsId() const
{
    return QString("setup-max-x");
}

/*!
 * Gives the setup's max-y value statistics id.
 */
QString ZoneBasedFishModel::maxYStatisticsId() const
{
    return QString("setup-max-y");
}

/*!
 * Gives the setup's min-x value statistics id.
 */
QString ZoneBasedFishModel::minXStatisticsId() const
{
    return QString("setup-min-x");
}

/*!
 * Gives the setup's min-y value statistics id.
 */
QString ZoneBasedFishModel::minYStatisticsId() const
{
    return QString("setup-min-y");
}
