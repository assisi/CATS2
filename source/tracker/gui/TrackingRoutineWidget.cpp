#include "TrackingRoutineWidget.hpp"
#include "ui_TrackingRoutineWidget.h"

/*!
 * Constructor.
 */
TrackingRoutineWidget::TrackingRoutineWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::TrackingRoutineWidget)
{
    m_ui->setupUi(this);
}

/*!
 * Destructor.
 */
TrackingRoutineWidget::~TrackingRoutineWidget()
{
    delete m_ui;
}
