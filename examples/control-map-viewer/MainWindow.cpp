#include "ui_MainWindow.h"
#include "MainWindow.hpp"

#include "ViewerHandler.hpp"
#include "gui/ViewerWidget.hpp"
#include "ViewerData.hpp"
#include <settings/CommandLineParameters.hpp>
#include <settings/ReadSettingsHelper.hpp>
#include <settings/RobotControlSettings.hpp>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QStatusBar>

/*!
 * Constructor.
 */
MainWindow::MainWindow(SetupType::Enum setupType,
                       TimestampedFrameQueuePtr queuePtr,
                       CoordinatesConversionPtr coordinatesConversion,
                       QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    // and the viewer handler
    m_viewerHandler = ViewerHandlerPtr(new ViewerHandler(setupType, queuePtr, coordinatesConversion));
    m_viewerHandler->data()->blockSignals(true);
    // make the frame viewer the central widget
    setCentralWidget(m_viewerHandler->widget());
    connect(m_viewerHandler->widget(), &ViewerWidget::mousePosition,
            [this](PositionPixels imagePosition, PositionMeters worldPosition) {
        statusBar()->showMessage(tr("Image position: %1, world position: %2")
                                 .arg(imagePosition.toString())
                                 .arg(worldPosition.toString()));
    });
    m_viewerHandler->data()->blockSignals(false);

    // make a new controller
    m_viewerHandler->widget()->updateCurrentAgent("Z");
    ExperimentControllerSettingsPtr settings =
            RobotControlSettings::get().controllerSettings(ExperimentControllerType::CONTROL_MAP);
    m_mapController = QSharedPointer<MapController>(new MapController(nullptr,
                                                                      settings));
    connect(m_mapController.data(), &ExperimentController::notifyPolygons,
            [=](QList<AnnotatedPolygons> polygons) {
                m_viewerHandler->widget()->updateControlAreas("Z", polygons);
            });
    // send out polygons to draw
    m_mapController->requestPolygons();

    // connect the window's actions
    connect(m_ui->actionZoomIn, &QAction::triggered,
            m_viewerHandler->widget(), &ViewerWidget::onZoomIn);
    connect(m_ui->actionZoomOut, &QAction::triggered,
            m_viewerHandler->widget(), &ViewerWidget::onZoomOut);
    connect(m_ui->actionShowControlAreas, &QAction::toggled,
            m_viewerHandler->widget(), &ViewerWidget::setShowControlAreas);
    connect(m_ui->actionShowControlAreas, &QAction::toggled,
            m_mapController.data(), &ExperimentController::requestPolygons);
    connect(m_ui->actionAdjustView, &QAction::triggered,
            m_viewerHandler->widget(), &ViewerWidget::adjust);
}

/*!
 * Destructor.
 */
MainWindow::~MainWindow()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
}
