#include "TrackingRoutineWidget.hpp"
#include "ui_TrackingRoutineWidget.h"

TrackingRoutineWidget::TrackingRoutineWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrackingRoutineWidget)
{
    ui->setupUi(this);
}

TrackingRoutineWidget::~TrackingRoutineWidget()
{
    delete ui;
}
