#include "ui_MainWindow.h"
#include "MainWindow.hpp"

#include <TrackingSetup.hpp>

#include <QtCore/QCoreApplication>
#include <QtGui/QtGui>

/*!
 * Constructor.
 */
MainWindow::MainWindow(SetupType::Enum setupType, QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
//    setWindowIcon(QIcon(":/images/mobots_logo.png"));

    bool needOutputQueue = true;
    m_cameraSetup = TrackingSetupPtr(new TrackingSetup(setupType, needOutputQueue));

    setCentralWidget(m_cameraSetup->trackingWidget());
}

/*!
 * Destructor.
 */
MainWindow::~MainWindow()
{

}
