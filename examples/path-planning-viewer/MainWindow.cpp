#include "ui_MainWindow.h"
#include "MainWindow.hpp"

#include "ViewerHandler.hpp"
#include "gui/ViewerWidget.hpp"
#include "ViewerData.hpp"
#include <settings/CommandLineParameters.hpp>
#include <settings/ReadSettingsHelper.hpp>

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
    m_ui(new Ui::MainWindow),
    m_pathPlanner(),
    m_startPosition(),
    m_goalPosition()
{
    m_ui->setupUi(this);

    // and the viewer handler
    m_viewerHandler = ViewerHandlerPtr(new ViewerHandler(setupType, queuePtr, coordinatesConversion));
    m_viewerHandler->data()->blockSignals(true);
    // make the frame viewer the central widget
    setCentralWidget(m_viewerHandler->widget());
    connect(m_viewerHandler->widget(), &ViewerWidget::mousePosition, [this](PositionPixels imagePosition, PositionMeters worldPosition) {
        statusBar()->showMessage(tr("Image position: %1, world position: %2").arg(imagePosition.toString()).arg(worldPosition.toString()));
    });
    m_viewerHandler->data()->blockSignals(false);

    // connect the window's actions
    connect(m_ui->actionZoomIn, &QAction::triggered, m_viewerHandler->widget(), &ViewerWidget::onZoomIn);
    connect(m_ui->actionZoomOut, &QAction::triggered, m_viewerHandler->widget(), &ViewerWidget::onZoomOut);
    connect(m_ui->actionOpenPathPlanningConfig, &QAction::triggered, this, &MainWindow::openControlMap);
    connect(m_ui->actionAdjustView, &QAction::triggered, m_viewerHandler->widget(), &ViewerWidget::adjust);
    connect(m_ui->actionSaveCurrentView, &QAction::triggered, m_viewerHandler->widget(), &ViewerWidget::saveCurrentFrameToFile);

    connect(m_viewerHandler->widget(), &ViewerWidget::notifyButtonClick, this, &MainWindow::onButtonClicked);

    m_startPosition.setValid(false);
    m_goalPosition.setValid(false);
}

/*!
 * Destructor.
 */
MainWindow::~MainWindow()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
}

/*!
 * Open control map.
 */
void MainWindow::openControlMap()
{
    // get the file name
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose File"),"",tr("XML files (*.xml)"));
    if (!fileName.isEmpty()){
        // first clean up
        if (m_pathPlanner.data())
            m_pathPlanner.reset();
        // now make a new one
        m_pathPlanner = QSharedPointer<DijkstraPathPlanner>(new DijkstraPathPlanner(fileName));
        m_viewerHandler->widget()->showAreas(true);
        // show the path planning area
        QList<AnnotatedPolygons> polygons;
        AnnotatedPolygons annotatedPolygons;
        annotatedPolygons.color = QColor(Qt::red);
        annotatedPolygons.label = "Working space";
        annotatedPolygons.polygons.append(m_pathPlanner->polygon());
        polygons.append(annotatedPolygons);
        m_viewerHandler->widget()->updateAreas(polygons);
    }
}

/*!
 * Sets the start and goal positions.
 */
void MainWindow::onButtonClicked(Qt::MouseButton button,PositionMeters worldPosition)
{
    // update the positions
    if (button == Qt::LeftButton)
        m_startPosition = worldPosition;
    else if (button == Qt::RightButton)
        m_goalPosition = worldPosition;
    // a hack to visualize the positions
    QList<AgentDataWorld> points;
    points << AgentDataWorld("Start", AgentType::GENERIC, StateWorld(m_startPosition));
    points << AgentDataWorld("Goal", AgentType::GENERIC, StateWorld(m_goalPosition));
    m_viewerHandler->widget()->updateAgentLabels(points);

    if (m_pathPlanner.data() && m_pathPlanner->isValid()
            && m_startPosition.isValid() && m_goalPosition.isValid()) {
        QQueue<PositionMeters> path = m_pathPlanner->plan(m_startPosition, m_goalPosition);
        m_viewerHandler->widget()->updateTrajectory(path);
    }
}
