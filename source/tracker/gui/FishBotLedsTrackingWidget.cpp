#include "FishBotLedsTrackingWidget.hpp"
#include "ui_FishBotLedsTrackingWidget.h"

#include <QtCore/QDebug>

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
    qDebug() << "Destroying the object";
    delete m_ui;
}
