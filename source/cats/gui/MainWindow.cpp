#include "ui_MainWindow.h"
#include "MainWindow.hpp"
#include "settings/Settings.hpp"
#include <settings/InterSpeciesSettings.hpp>

#include <TrackingSetup.hpp>
#include <TrackingDataManager.hpp>
#include <ViewerHandler.hpp>
#include <ViewerData.hpp>
#include <gui/ViewerWidget.hpp>

#include <QtCore/QCoreApplication>
#include <QtGui/QtGui>
#include <QtCore/QTimer>
#include <QtWidgets/QStatusBar>

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

    // create the tracking data manager
    m_trackingDataManager = TrackingDataManagerPtr(new TrackingDataManager(), &QObject::deleteLater);

    // create the inter-species data manager
    m_interSpeciesDataManager = InterSpeciesDataManagerPtr(new InterSpeciesDataManager(InterSpeciesSettings::get().publisherAddress()), &QObject::deleteLater);
    connect(m_trackingDataManager.data(), &TrackingDataManager::notifyAgentDataImageMerged,
               m_interSpeciesDataManager.data(), &InterSpeciesDataManager::publishAgentData);

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
}

/*!
 * Destructor.
 */
MainWindow::~MainWindow()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
}

/*!
 * Creates the tracking setup and corresponding viewers of requested type.
 */
void MainWindow::createSetup(SetupType::Enum setupType)
{
    // create the tracking setup.
    bool needOutputQueue = true;
    m_trackingSetups[setupType] = TrackingSetupPtr(new TrackingSetup(setupType, needOutputQueue));

    // create the viewer
    m_viewerHandlers[setupType] = ViewerHandlerPtr(new ViewerHandler(setupType,
                                                                     m_trackingSetups[setupType]->viewerQueue(),
                                                                     m_trackingSetups[setupType]->coordinatesConversion()),
                                            &QObject::deleteLater);

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
        QGridLayout *layout = new QGridLayout(m_ui->primaryViewerWidget);
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
        m_viewerHandlers[setupType]->data()->blockSignals(false);
    } else {
        qDebug() << Q_FUNC_INFO << "Unable to set the primary view from setup:" << SetupType::toString(setupType);
    }
}

/*!
 * Sets the secondary viewer on the left side of the window.
 */
void MainWindow::setSecondaryView(SetupType::Enum setupType)
{
    // create the layout if it does not exist
    if (!m_ui->secondaryViewerWidget->layout()) {
        QGridLayout *layout = new QGridLayout(m_ui->secondaryViewerWidget);
    }
    // set new secondary view
    if (m_viewerHandlers.contains(setupType)) {
        m_viewerHandlers[setupType]->data()->blockSignals(true); // to avoid painting on the "moving" widget
        m_secondarySetupType = setupType;
        m_ui->secondaryViewerWidget->layout()->addWidget(m_viewerHandlers[setupType]->widget());
        // run the timer to adjust the size of the view to the available area
        QTimer::singleShot(1000, Qt::PreciseTimer, [=]{ m_viewerHandlers[setupType]->widget()->adjust(); } );
        m_viewerHandlers[setupType]->data()->blockSignals(false);
    } else {
        qDebug() << Q_FUNC_INFO << "Unable to set the secondary view from setup:" << SetupType::toString(setupType);
    }
}


/*!
 * Connects the signals and slots of the primary view.
 */
void MainWindow::connectPrimaryView()
{
    if (m_primarySetupType != SetupType::UNDEFINED) {
        ViewerWidget* viewerWidget = m_viewerHandlers[m_primarySetupType]->widget();
        connect(viewerWidget, &ViewerWidget::mousePosition, [this](PositionPixels imagePosition, PositionMeters worldPosition) {
            statusBar()->showMessage(tr("Image position: %1, world position: %2").arg(imagePosition.toString()).arg(worldPosition.toString()));
        });

        // connect the window's actions
        connect(m_ui->actionZoomIn, &QAction::triggered, viewerWidget, &ViewerWidget::onZoomIn);
        connect(m_ui->actionZoomOut, &QAction::triggered, viewerWidget, &ViewerWidget::onZoomOut);
        connect(m_ui->actionSaveCurrentView, &QAction::triggered, viewerWidget, &ViewerWidget::saveCurrentFrameToFile);
        connect(m_ui->actionAdjustView, &QAction::triggered, viewerWidget, &ViewerWidget::adjust);

        // connect to the tracking data manager
        connect(m_trackingDataManager.data(), &TrackingDataManager::notifyAgentDataWorldMerged,
                viewerWidget, &ViewerWidget::showAgents);
    }
}

/*!
 * Disconnects all the signals and slots of the primary view.
 */
void MainWindow::disconnectPrimaryView()
{
    if (m_primarySetupType != SetupType::UNDEFINED) {
        ViewerHandlerPtr viewerHandler = m_viewerHandlers[m_primarySetupType];
        viewerHandler->widget()->disconnect();

        // disconnect the window's actions
        disconnect(m_ui->actionZoomIn, &QAction::triggered, viewerHandler->widget(), &ViewerWidget::onZoomIn);
        disconnect(m_ui->actionZoomOut, &QAction::triggered, viewerHandler->widget(), &ViewerWidget::onZoomOut);
        disconnect(m_ui->actionSaveCurrentView, &QAction::triggered, viewerHandler->widget(), &ViewerWidget::saveCurrentFrameToFile);
        disconnect(m_ui->actionAdjustView, &QAction::triggered, viewerHandler->widget(), &ViewerWidget::adjust);
    }
}

/*!
 * Adds the tracking settings to the tab on the left.
 */
void MainWindow::addTrackingSettingsWidget(SetupType::Enum setupType)
{
    m_ui->trackingSettingsWidget->addTab(m_trackingSetups[setupType]->trackingWidget(), SetupType::toString(setupType));
}
