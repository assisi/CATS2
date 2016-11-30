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
    m_ui(new Ui::MainWindow)
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
    connect(m_ui->actionOpenControlMap, &QAction::triggered, this, &MainWindow::openControlMap);
    connect(m_ui->actionAdjustView, &QAction::triggered, m_viewerHandler->widget(), &ViewerWidget::adjust);
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
        if (m_controlMap.data())
            m_controlMap.reset();
        // now make a new one
        m_controlMap = QSharedPointer<ControlMap>(new ControlMap(fileName));
        connect(m_controlMap.data(), &ControlMap::notifyPolygons, m_viewerHandler->widget(), &ViewerWidget::updateAreas);
        m_viewerHandler->widget()->showAreas(true);
        // send out polygons to draw
        m_controlMap->requestPolygons();
    }
}
