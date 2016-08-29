#include "TrackingRoutineWidget.hpp"
#include "ui_TrackingRoutineWidget.h"

#include "TrackingData.hpp"
#include "TrackingUiFactory.hpp"

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

    if (!m_data.isNull()) {
        m_ui->setupLabel->setText(SetupType::toString(m_data->setupType()));
        m_ui->trackingTypeLabel->setText(TrackingRoutineType::toString(m_data->trackingType()));
    }

    // show the debug viewer window
    connect(m_ui->showViewerButton, &QPushButton::toggled, this, &TrackingRoutineWidget::onShowDebugViewer);
    // request to start feeding the debug queue
    connect(m_ui->showViewerButton, &QPushButton::toggled, m_data.data(), &TrackingData::sendDebugImages);

    // create the specific settings widget
    QWidget* specificWidget = TrackingUiFactory::createWidget(trackingData);
    if (specificWidget) {
        if (!m_ui->settingsWidget->layout()) {
            QHBoxLayout* layout = new QHBoxLayout(m_ui->settingsWidget);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(3);
        }
        m_ui->settingsWidget->layout()->addWidget(specificWidget);
        m_ui->settingsWidget->hide();
        connect(m_ui->showDetailsButton, &QPushButton::toggled, [=](bool checked){ m_ui->settingsWidget->setVisible(checked); });
    }
}

/*!
 * Destructor.
 */
TrackingRoutineWidget::~TrackingRoutineWidget()
{
    // we don't care anymore if the window is closed
    // (also sometimes the lambda-slot from below makes the app crash
    if (m_viewerWindow)
        m_viewerWindow->disconnect();
    // delete gui
    delete m_ui;
}

/*!
 * Shows/hides the debug viewer window.
 */
void TrackingRoutineWidget::onShowDebugViewer(bool showWindow)
{
    if (!m_data.isNull()) {
        if (showWindow) {
            m_viewerWindow = new ViewerWindow(m_data->setupType(), m_data->debugQueue(), m_data->coordinatesConversion(), this);
            m_viewerWindow->setAttribute(Qt::WA_DeleteOnClose, true); // delete the window once it is closed
            // when the window is about to be deleted we clean the pointer and uncheck the button
            connect(m_viewerWindow, &ViewerWindow::destroyed, [this](QObject *object) {
                m_viewerWindow = nullptr;
                m_ui->showViewerButton->blockSignals(true);
                m_ui->showViewerButton->setChecked(false);
                m_ui->showViewerButton->blockSignals(false);
            });
            m_viewerWindow->show();
        } else {
            m_viewerWindow->deleteLater();
        }
    }
}
