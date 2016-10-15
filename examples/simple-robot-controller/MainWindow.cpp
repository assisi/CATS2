#include "ui_MainWindow.h"
#include "MainWindow.hpp"

#include <RobotsHandler.hpp>

#include <QtCore/QCoreApplication>
#include <QtGui/QtGui>

/*!
 * Constructor.
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    m_robotsHandler = RobotsHandlerPtr(new RobotsHandler());

    setCentralWidget(m_robotsHandler->widget());
}

/*!
 * Destructor.
 */
MainWindow::~MainWindow()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
}
