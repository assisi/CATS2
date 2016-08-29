#include "FishBotLedsTrackingWidget.hpp"
#include "ui_FishBotLedsTrackingWidget.h"

/*!
 * Constructor.
 */
FishBotLedsTrackingWidget::FishBotLedsTrackingWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::FishBotLedsTrackingWidget)
{
    m_ui->setupUi(this);
}

/*!
 * Destructor.
 */
FishBotLedsTrackingWidget::~FishBotLedsTrackingWidget()
{
    delete m_ui;
}
