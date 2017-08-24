#include "ui_MainWindow.h"
#include "MainWindow.hpp"

#include "FrameScene.hpp"
#include <QtWidgets/QGraphicsPixmapItem>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QStatusBar>

/*!
 * Constructor.
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    // always receive mouse events
    m_ui->view->setMouseTracking(true);

    // create the scene
    m_scene = new FrameScene(this);
    connect(m_scene, &FrameScene::mouseMoved, this, &MainWindow::onMouseMoved);
    connect(m_scene, &FrameScene::mouseClicked, this, &MainWindow::onMouseClicked);
    m_ui->view->setScene(m_scene);

    // create the video frame item
    m_videoFrame = new QGraphicsPixmapItem();
    m_videoFrame->setZValue(0);
    m_videoFrame->setPos(0,0);
    m_scene->addItem(m_videoFrame);

    connect(m_ui->actionOpenFile, &QAction::triggered, this, &MainWindow::openFile);
    connect(m_ui->actionZoomIn, &QAction::triggered, this, &MainWindow::onZoomIn);
    connect(m_ui->actionZoomOut, &QAction::triggered, this, &MainWindow::onZoomOut);
    connect(m_ui->actionAdjustView, &QAction::triggered, this, &MainWindow::adjust);
}

/*!
 * Destructor.
 */
MainWindow::~MainWindow()
{
    qDebug() << "Destroying the object";
    delete m_ui;
}

/*!
 * Open file.
 */
void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose File"),"",tr("Image Files (*.png *.jpg *.bmp)"));
    if (!fileName.isEmpty()){
        QPixmap pixmap;
        pixmap.load(fileName);
        m_videoFrame->setPixmap(pixmap);
    }
}

/*!
 * Triggered when a new mouse position is received from the scene.
 */
void MainWindow::onMouseMoved(QPointF scenePosition)
{
    statusBar()->showMessage(tr("Image position: %1 px, %2 px").arg(scenePosition.x()).arg(scenePosition.y()));
}


/*!
 * Zoom on the video.
 */
void MainWindow::onZoomIn()
{
    m_ui->view->scale(1.2, 1.2);
}

/*!
 * Unzoom the video.
 */
void MainWindow::onZoomOut()
{
    m_ui->view->scale(1/1.2, 1/1.2);
}

/*!
 * Scale to the available area.
 */
void MainWindow::adjust()
{
    QSize availableArea = m_ui->view->size();
    float width = m_ui->view->transform().m11() * m_videoFrame->pixmap().width();
    float height = m_ui->view->transform().m22() * m_videoFrame->pixmap().height();
    double scaleCoefficient = qMin(availableArea.width() / width,
                                   availableArea.height() / height);
    m_ui->view->scale(scaleCoefficient, scaleCoefficient);
}

/*!
 * Triggered when a mouse click position is received from the scene.
 */
void MainWindow::onMouseClicked(QPointF scenePosition)
{
    qDebug() << scenePosition;
    m_ui->xEdit->setText(QString::number(scenePosition.x()));
    m_ui->yEdit->setText(QString::number(scenePosition.y()));
}

