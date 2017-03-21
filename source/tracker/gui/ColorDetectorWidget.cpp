#include "ColorDetectorWidget.hpp"
#include "ui_ColorDetectorWidget.h"

#include <settings/ColorDetectorSettings.hpp>
#include <routines/ColorDetector.hpp>

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
ColorDetectorWidget::ColorDetectorWidget(TrackingRoutinePtr routine, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ColorDetectorWidget),
    m_routine(routine)
{
    m_ui->setupUi(this);
}

/*!
 * Destructor.
 */
ColorDetectorWidget::~ColorDetectorWidget()
{
    qDebug() << "Destroying the object";
    delete m_ui;
}
