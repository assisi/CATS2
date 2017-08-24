#include "ui_ViewerWindow.h"
#include "ViewerWindow.hpp"

#include "ViewerHandler.hpp"

#include "gui/ViewerWidget.hpp"
#include "ViewerData.hpp"
#include <settings/CommandLineParameters.hpp>

#include <QtCore/QCoreApplication>
#include <QtGui/QtGui>

/*!
 * Constructor.
 */
ViewerWindow::ViewerWindow(SetupType::Enum setupType,
                           TimestampedFrameQueuePtr queuePtr,
                           CoordinatesConversionPtr coordinatesConversion,
                           QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::ViewerWindow)
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
    connect(m_ui->actionSaveCurrentView, &QAction::triggered, m_viewerHandler->widget(), &ViewerWidget::saveCurrentFrameToFile);
}

/*!
 * Destructor.
 */
ViewerWindow::~ViewerWindow()
{
    qDebug() << "Destroying the object";
    delete m_ui;
}

/*!
 * Sets the flag in the viewer widget to show the framerate.
 */
void ViewerWindow::setShowFrameRate(bool value)
{
    m_viewerHandler->widget()->setShowFrameRate(value);
}

/*!
 * Sets the flag in the viewer widget to automatically resize to the
 * window size.
 */
void ViewerWindow::setAutoAdjust(bool value)
{
    m_viewerHandler->widget()->setAutoAdjust(value);
}
