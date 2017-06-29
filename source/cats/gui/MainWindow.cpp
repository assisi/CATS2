#include "ui_MainWindow.h"
#include "MainWindow.hpp"

#include "PreferencesDialog.hpp"

#include "settings/Settings.hpp"
#include "settings/Registry.hpp"
#include <settings/InterSpeciesSettings.hpp>

#include <TrackingSetup.hpp>
#include <TrackingDataManager.hpp>
#include <ViewerHandler.hpp>
#include <ViewerData.hpp>
#include <DebugLogger.hpp>
#include <gui/ViewerWidget.hpp>

#include <RobotsHandler.hpp>
#include <ControlLoop.hpp>
#include <settings/RobotControlSettings.hpp>

#include <QtCore/QCoreApplication>
#include <QtGui/QtGui>
#include <QtCore/QTimer>
#include <QtWidgets/QStatusBar>

static DebugLogger logger;
void messageOutput(QtMsgType type,
                   const QMessageLogContext &context,
                   const QString &msg);

/*!
 * Constructor.
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_primarySetupType(SetupType::UNDEFINED),
    m_secondarySetupType(SetupType::UNDEFINED)
{
    m_ui->setupUi(this);

    // install the logger
    logger.init();
    qInstallMessageHandler(messageOutput);

    // create the tracking data manager
    QString path = Registry::get().dataLoggingPath();
    m_trackingDataManager = TrackingDataManagerPtr(new TrackingDataManager(path));

    // create the inter-species data manager
    m_interSpeciesDataManager =
            InterSpeciesDataManagerPtr(new InterSpeciesDataManager(InterSpeciesSettings::get().publisherAddress()));
    connect(m_trackingDataManager.data(),
            &TrackingDataManager::notifyAgentDataImageMerged,
            m_interSpeciesDataManager.data(),
            &InterSpeciesDataManager::publishAgentData);

    // create the robot control handler
    m_robotsHandler = RobotsHandlerPtr(new RobotsHandler());
    if (RobotControlSettings::get().numberOfRobots() > 0) {
        // add a layout
        new QGridLayout(m_ui->robotsControllerWidget);
        // and place a robot controller widget on this layout
        m_ui->robotsControllerWidget->layout()->addWidget(m_robotsHandler->widget());
    }
    connect(m_trackingDataManager.data(),
            &TrackingDataManager::notifyAgentDataWorldMerged,
            m_robotsHandler->contolLoop().data(),
            &ControlLoop::onTrackingResultsReceived);
    connect(m_ui->actionReconnectToRobots, &QAction::triggered,
            m_robotsHandler->contolLoop().data(), &ControlLoop::reconnectRobots);
    connect(m_ui->actionStopAllRobots, &QAction::triggered,
            m_robotsHandler->contolLoop().data(), &ControlLoop::stopAllRobots);

    // create setups
    if (Settings::get().isAvailable(SetupType::MAIN_CAMERA)) {
        createSetup(SetupType::MAIN_CAMERA);
        addTrackingSettingsWidget(SetupType::MAIN_CAMERA);
        setPrimaryView(SetupType::MAIN_CAMERA);
    }

    if (Settings::get().isAvailable(SetupType::CAMERA_BELOW)) {
        createSetup(SetupType::CAMERA_BELOW);
        addTrackingSettingsWidget(SetupType::CAMERA_BELOW);
        // set it as either primary or secondary viewer
        if (m_primarySetupType == SetupType::UNDEFINED)
            setPrimaryView(SetupType::CAMERA_BELOW);
        else
            setSecondaryView(SetupType::CAMERA_BELOW);
    }

    // show the window maximazed
     setWindowState(Qt::WindowMaximized);

     // automatically toogle some of the toolbar buttons
     m_ui->actionShowAgentsData->toggle();
     m_ui->actionShowControlAreas->toggle();
     m_ui->actionShowSetupOutline->toggle();

     // preferences dialog
     connect(m_ui->actionPreferences, &QAction::triggered,
             [=](){
                QSharedPointer<PreferencesDialog> dialog(new PreferencesDialog());
                dialog->exec();
             });
}

/*!
 * Destructor.
 */
MainWindow::~MainWindow()
{
    qDebug() << "Destroying the object";
}

/*!
 * Creates the tracking setup and corresponding viewers of requested type.
 */
void MainWindow::createSetup(SetupType::Enum setupType)
{
    // create the tracking setup.
    bool needOutputQueue = true;
    m_trackingSetups[setupType] =
            TrackingSetupPtr(new TrackingSetup(setupType, needOutputQueue));

    // create the viewer
    m_viewerHandlers[setupType] =
            ViewerHandlerPtr(new ViewerHandler(setupType,
                                               m_trackingSetups[setupType]->viewerQueue(),
                                               m_trackingSetups[setupType]->coordinatesConversion()));

    // connect the tracker to the tracking data manager
    m_trackingSetups[setupType]->connectToDataManager(m_trackingDataManager);
}

/*!
 * Sets the primary viewer.
 */
void MainWindow::setPrimaryView(SetupType::Enum setupType)
{
    // create the layout if it does not exist
    if (!m_ui->primaryViewerWidget->layout()) {
        new QGridLayout(m_ui->primaryViewerWidget);
    }
    // if there is another primary view then disonnect its signals and slots
    disconnectPrimaryView();
    // set new primary view
    if (m_viewerHandlers.contains(setupType)) {
        m_viewerHandlers[setupType]->data()->blockSignals(true); // to avoid painting on the "moving" widget
        m_primarySetupType = setupType;
        connectPrimaryView();
        m_ui->primaryViewerWidget->layout()->addWidget(m_viewerHandlers[setupType]->widget());
        // run the timer to adjust the size of the view to the available area
        QTimer::singleShot(1000, Qt::PreciseTimer, [=]{ m_viewerHandlers[setupType]->widget()->adjust(); } );
        m_viewerHandlers[setupType]->widget()->setAutoAdjust(false);
        m_viewerHandlers[setupType]->widget()->setShowRunTime(true);
        m_viewerHandlers[setupType]->data()->blockSignals(false);
    } else {
        qDebug() << "Unable to set the primary view from setup:"
                 << SetupType::toString(setupType);
    }
}

/*!
 * Sets the secondary viewer on the left side of the window.
 */
void MainWindow::setSecondaryView(SetupType::Enum setupType)
{
    // create the layout if it does not exist
    if (!m_ui->secondaryViewerWidget->layout()) {
        new QGridLayout(m_ui->secondaryViewerWidget);
    }
    // set new secondary view
    if (m_viewerHandlers.contains(setupType)) {
        m_viewerHandlers[setupType]->data()->blockSignals(true); // to avoid painting on the "moving" widget
        m_secondarySetupType = setupType;
        m_ui->secondaryViewerWidget->layout()->addWidget(m_viewerHandlers[setupType]->widget());
        // run the timer to adjust the size of the view to the available area
        QTimer::singleShot(1000, Qt::PreciseTimer, [=]{ m_viewerHandlers[setupType]->widget()->adjust(); } );
        m_viewerHandlers[setupType]->widget()->setAutoAdjust(true);
        m_viewerHandlers[setupType]->widget()->setShowRunTime(false);
        m_viewerHandlers[setupType]->data()->blockSignals(false);
    } else {
        qDebug() << "Unable to set the secondary view from setup:"
                 << SetupType::toString(setupType);
    }
}

/*!
 * Connects the signals and slots of the primary view.
 */
void MainWindow::connectPrimaryView()
{
    if (m_primarySetupType != SetupType::UNDEFINED) {
        ViewerWidget* viewerWidget = m_viewerHandlers[m_primarySetupType]->widget();
        connect(viewerWidget, &ViewerWidget::mousePosition,
                [this](PositionPixels imagePosition, PositionMeters worldPosition)
        {
            statusBar()->showMessage(tr("Image position: %1, world position: %2")
                                     .arg(imagePosition.toString())
                                     .arg(worldPosition.toString()));
        });

        // connect the window's actions
        connect(m_ui->actionZoomIn, &QAction::triggered,
                viewerWidget, &ViewerWidget::onZoomIn);
        connect(m_ui->actionZoomOut, &QAction::triggered,
                viewerWidget, &ViewerWidget::onZoomOut);
        connect(m_ui->actionSaveCurrentView, &QAction::triggered,
                viewerWidget, &ViewerWidget::saveCurrentFrameToFile);
        connect(m_ui->actionAdjustView, &QAction::triggered,
                viewerWidget, &ViewerWidget::adjust);
        connect(m_ui->actionShowAgentsData, &QAction::toggled,
                viewerWidget, &ViewerWidget::setShowAgentsData);
        connect(m_ui->actionShowAgentsData, &QAction::toggled,
                m_robotsHandler->contolLoop().data(), &ControlLoop::sendNavigationData);
        connect(m_ui->actionShowControlAreas, &QAction::toggled,
                viewerWidget, &ViewerWidget::setShowControlAreas);
        connect(m_ui->actionShowControlAreas, &QAction::toggled,
                m_robotsHandler->contolLoop().data(), &ControlLoop::sendControlAreas);
        connect(m_ui->actionShowSetupOutline, &QAction::toggled,
                viewerWidget, &ViewerWidget::setShowSetup);
        connect(m_ui->actionShowSetupOutline, &QAction::toggled,
                m_robotsHandler.data(), &RobotsHandler::requestSetupMap);

        // connect to the tracking data manager
        connect(m_trackingDataManager.data(), &TrackingDataManager::notifyAgentDataWorldMerged,
                viewerWidget, &ViewerWidget::updateAgentLabels);
        connect(m_trackingDataManager.data(), &TrackingDataManager::notifyAgentDataWorldMerged,
                viewerWidget, &ViewerWidget::updateAgents);

        // connect to the robots controller
        connect(viewerWidget, &ViewerWidget::notifyButtonClick,
                [=](Qt::MouseButton button, PositionMeters worldPosition)
                {
                    if (button == Qt::RightButton)
                        m_robotsHandler->contolLoop().data()->goToPosition(worldPosition);
                });
        connect(m_robotsHandler->contolLoop().data(),
                &ControlLoop::notifyRobotControlAreasPolygons,
                viewerWidget,
                &ViewerWidget::updateControlAreas);
        connect(m_robotsHandler->contolLoop().data(),
                &ControlLoop::notifyFishNumberByRobotControlAreas,
                viewerWidget,
                &ViewerWidget::updateControlAreasOccupation);
        connect(m_robotsHandler->contolLoop().data(),
                &ControlLoop::notifySelectedRobotChanged,
                viewerWidget,
                &ViewerWidget::updateCurrentAgent);
        connect(m_robotsHandler->contolLoop().data(),
                &ControlLoop::notifyRobotTargetPositionChanged,
                viewerWidget,
                &ViewerWidget::updateTarget);
        connect(m_robotsHandler->contolLoop().data(),
                &ControlLoop::notifyRobotTrajectoryChanged,
                viewerWidget,
                &ViewerWidget::updateTrajectory);
        connect(m_robotsHandler.data(), &RobotsHandler::notifySetupMap,
                viewerWidget, &ViewerWidget::updateSetup);
        connect(m_robotsHandler->contolLoop().data(),
                &ControlLoop::notifyRobotLedColor,
                viewerWidget,
                &ViewerWidget::updateColor);
        connect(m_robotsHandler->contolLoop().data(),
                &ControlLoop::notifyObstacleDetectedStatusChanged,
                viewerWidget,
                &ViewerWidget::highlightAgent);

        // request to get robots leds' colors
        m_robotsHandler->contolLoop()->requestRobotsLedColors();
        // request to get the current robot
        m_robotsHandler->contolLoop()->requestSelectedRobot();
    }
}

/*!
 * Disconnects all the signals and slots of the primary view.
 */
void MainWindow::disconnectPrimaryView()
{
    if (m_primarySetupType != SetupType::UNDEFINED) {
        ViewerWidget* viewerWidget = m_viewerHandlers[m_primarySetupType]->widget();
        // disconnect all signals comming from the viewer
        viewerWidget->disconnect();

        // disconnect the window's actions
        disconnect(m_ui->actionZoomIn, &QAction::triggered,
                   viewerWidget, &ViewerWidget::onZoomIn);
        disconnect(m_ui->actionZoomOut, &QAction::triggered,
                   viewerWidget, &ViewerWidget::onZoomOut);
        disconnect(m_ui->actionSaveCurrentView, &QAction::triggered,
                   viewerWidget, &ViewerWidget::saveCurrentFrameToFile);
        disconnect(m_ui->actionAdjustView, &QAction::triggered,
                   viewerWidget, &ViewerWidget::adjust);
        disconnect(m_ui->actionShowAgentsData, &QAction::toggled,
                   viewerWidget, &ViewerWidget::setShowAgentsData);
        disconnect(m_ui->actionShowAgentsData,
                   &QAction::toggled,
                   m_robotsHandler->contolLoop().data(),
                   &ControlLoop::sendNavigationData);
        disconnect(m_ui->actionShowControlAreas, &QAction::toggled,
                   viewerWidget, &ViewerWidget::setShowControlAreas);
        disconnect(m_ui->actionShowControlAreas, &QAction::toggled,
                   m_robotsHandler->contolLoop().data(),
                   &ControlLoop::sendControlAreas);
        disconnect(m_ui->actionShowSetupOutline,
                   &QAction::toggled,
                   viewerWidget, &ViewerWidget::setShowSetup);
        disconnect(m_ui->actionShowSetupOutline, &QAction::toggled,
                   m_robotsHandler.data(), &RobotsHandler::requestSetupMap);

        // disconnect from the tracking data manager
        disconnect(m_trackingDataManager.data(),
                   &TrackingDataManager::notifyAgentDataWorldMerged,
                   viewerWidget,
                   &ViewerWidget::updateAgentLabels);
        disconnect(m_trackingDataManager.data(),
                   &TrackingDataManager::notifyAgentDataWorldMerged,
                   viewerWidget,
                   &ViewerWidget::updateAgents);

        // disconnect from the robot controller
        disconnect(m_robotsHandler->contolLoop().data(),
                   &ControlLoop::notifyRobotControlAreasPolygons,
                   viewerWidget,
                   &ViewerWidget::updateControlAreas);
        disconnect(m_robotsHandler->contolLoop().data(),
                &ControlLoop::notifyFishNumberByRobotControlAreas,
                viewerWidget,
                &ViewerWidget::updateControlAreasOccupation);
        disconnect(m_robotsHandler->contolLoop().data(),
                   &ControlLoop::notifySelectedRobotChanged,
                   viewerWidget,
                   &ViewerWidget::updateCurrentAgent);
        disconnect(m_robotsHandler->contolLoop().data(),
                   &ControlLoop::notifyRobotTargetPositionChanged,
                   viewerWidget,
                   &ViewerWidget::updateTarget);
        disconnect(m_robotsHandler->contolLoop().data(),
                   &ControlLoop::notifyRobotTrajectoryChanged,
                   viewerWidget,
                   &ViewerWidget::updateTrajectory);
        disconnect(m_robotsHandler.data(), &RobotsHandler::notifySetupMap,
                viewerWidget, &ViewerWidget::updateSetup);
        disconnect(m_robotsHandler->contolLoop().data(),
                   &ControlLoop::notifyObstacleDetectedStatusChanged,
                   viewerWidget,
                   &ViewerWidget::highlightAgent);
    }
}

/*!
 * Adds the tracking settings to the tab on the left.
 */
void MainWindow::addTrackingSettingsWidget(SetupType::Enum setupType)
{
    m_ui->trackingSettingsWidget->addTab(m_trackingSetups[setupType]->trackingWidget(),
                                         SetupType::toString(setupType));
}

void messageOutput(QtMsgType type,
                   const QMessageLogContext &context,
                   const QString &msg)
{
    logger.messageOutput(type, context, msg);
}
