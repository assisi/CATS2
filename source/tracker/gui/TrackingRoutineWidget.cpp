#include "TrackingRoutineWidget.hpp"
#include "ui_TrackingRoutineWidget.h"

#include "TrackingData.hpp"

#include <SetupType.hpp>

/*!
 * Constructor.
 */
TrackingRoutineWidget::TrackingRoutineWidget(TrackingDataPtr trackingData, QWidget *parent) :
    QWidget(parent),
    m_data(trackingData),
    m_viewerWindow(nullptr),
    m_ui(new Ui::TrackingRoutineWidget)
{
    m_ui->setupUi(this);

    m_ui->setupLabel->setText(SetupType::toString(m_data->setupType()));
    m_ui->trackingTypeLabel->setText(TrackingRoutineType::toString(m_data->trackingType()));

    connect(m_ui->showViewerButton, &QPushButton::toggled, this, &TrackingRoutineWidget::onShowDebugViewer);
}

/*!
 * Destructor.
 */
TrackingRoutineWidget::~TrackingRoutineWidget()
{
    delete m_ui;
}

/*!
 * Shows/hides the debug viewer window.
 */
void TrackingRoutineWidget::onShowDebugViewer(bool showWindow)
{
    if (showWindow) {
        m_viewerWindow = new ViewerWindow(m_data->debugQueue(), m_data->coordinatesConversion(), this);
        m_viewerWindow->setAttribute(Qt::WA_DeleteOnClose, true); // delete the window once it is closed
        // when the window is about to be deleted we clean the pointer and uncheck the button
        connect(m_viewerWindow, &ViewerWindow::destroyed, [this](QObject *object) {
            m_viewerWindow = nullptr;
            m_ui->showViewerButton->setChecked(false);
        });
        m_viewerWindow->show();
    } else {
        m_viewerWindow->deleteLater();
    }
}
