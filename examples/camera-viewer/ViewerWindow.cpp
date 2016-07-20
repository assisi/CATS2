#include "ui_ViewerWindow.h"
#include "ViewerWindow.hpp"

#include <ViewerWidget.hpp>
#include <ViewerHandler.hpp>
#include <VideoGrabber.hpp>
#include <CommandLineParameters.hpp>

#include <QtCore/QCoreApplication>
#include <QtGui/QtGui>

/*!
 * Constructor.
 */
ViewerWindow::ViewerWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::ViewerWindow)
{
    _ui->setupUi(this);
//	setWindowIcon(QIcon(":/images/mobots_logo.png"));

    TimestampedFrameQueuePtr queuePtr = TimestampedFrameQueuePtr(new TimestampedFrameQueue(100));

    // add the video grabber
    _grabber = new VideoGrabber(this);
    if (CommandLineParameters::get().mainCameraDescriptor().isValid()){
        _grabber->addStreamReceiver(CommandLineParameters::get().mainCameraDescriptor(), queuePtr);
    }

    // add the viewer gui
    _viewer = new ViewerWidget(this);
    setCentralWidget(_viewer);
    // and the viewer handler
    _viewerHandler = new ViewerHandler(queuePtr, _viewer, this);

    // connect the window's actions
    connect(_ui->actionZoomIn, &QAction::triggered, _viewer, &ViewerWidget::onZoomIn);
    connect(_ui->actionZoomOut, &QAction::triggered, _viewer, &ViewerWidget::onZoomOut);
}

/*!
 * Destructor.
 */
ViewerWindow::~ViewerWindow()
{

}
