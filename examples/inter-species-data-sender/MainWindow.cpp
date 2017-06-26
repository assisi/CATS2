#include "ui_MainWindow.h"
#include "MainWindow.hpp"

#include <InterSpeciesDataManager.hpp>
#include <settings/InterSpeciesSettings.hpp>

#include <QtCore/QCoreApplication>
#include <QtGui/QtGui>

#include <zmq.hpp>
#include <zmqHelpers.hpp>

/*!
 * Constructor.
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_context(1),
    m_publisher(m_context, ZMQ_PUB)
{
    m_ui->setupUi(this);
    QString subscriberAddress = InterSpeciesSettings::get().subscriberAddress();
    m_ui->bindLabel->setText("Binds:" + subscriberAddress);

    m_publisher.bind(subscriberAddress.toStdString().c_str());

    connect(&m_timer, &QTimer::timeout, [=](){
        zmq::sendMultipart(m_publisher, "ASSISIbf", "ASSISIbf", "ASSISIbf", "ASSISIbf");
        qDebug() << "Message is sent";
    });
    m_timer.start(1000);
}

/*!
 * Destructor.
 */
MainWindow::~MainWindow()
{
    qDebug() << "Destroying the object";
}
