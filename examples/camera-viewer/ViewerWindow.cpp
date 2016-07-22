#include "ui_ViewerWindow.h"
#include "ViewerWindow.hpp"

#include <gui/ViewerWidget.hpp>
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
    m_ui(new Ui::ViewerWindow)
{
    m_ui->setupUi(this);
//	setWindowIcon(QIcon(":/images/mobots_logo.png"));

    TimestampedFrameQueuePtr queuePtr = TimestampedFrameQueuePtr(new TimestampedFrameQueue(100));

    // add the video grabber
    m_grabber = QSharedPointer<VideoGrabber>(new VideoGrabber(), &QObject::deleteLater);
    if (CommandLineParameters::get().mainCameraDescriptor().isValid()){
        m_grabber->addStreamReceiver(CommandLineParameters::get().mainCameraDescriptor(), queuePtr);
    }

    // and the viewer handler
    m_viewerHandler = QSharedPointer<ViewerHandler>(new  ViewerHandler(queuePtr, this), &QObject::deleteLater);
    // make the frame viewer the central widget
    setCentralWidget(m_viewerHandler->widget());

    // connect the window's actions
    connect(m_ui->actionZoomIn, &QAction::triggered, m_viewerHandler->widget(), &ViewerWidget::onZoomIn);
    connect(m_ui->actionZoomOut, &QAction::triggered, m_viewerHandler->widget(), &ViewerWidget::onZoomOut);
    connect(m_ui->actionSaveCurrentView, &QAction::triggered, m_viewerHandler->widget(), &ViewerWidget::saveCurrentFrameToFile);
}

/*!
 * Destructor.
 */
ViewerWindow::~ViewerWindow()
{

}
