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

    // and the viewer handler
    _viewerHandler = new ViewerHandler(queuePtr, this);
    // make the frame viewer the central widget
    setCentralWidget(_viewerHandler->widget());

    // connect the window's actions
    connect(_ui->actionZoomIn, &QAction::triggered, _viewerHandler->widget(), &ViewerWidget::onZoomIn);
    connect(_ui->actionZoomOut, &QAction::triggered, _viewerHandler->widget(), &ViewerWidget::onZoomOut);
    connect(_ui->actionSaveCurrentView, &QAction::triggered, _viewerHandler->widget(), &ViewerWidget::saveCurrentFrameToFile);
}

/*!
 * Destructor.
 */
ViewerWindow::~ViewerWindow()
{

}
