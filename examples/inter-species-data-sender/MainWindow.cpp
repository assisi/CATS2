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
    QStringList subscriberAddresses = InterSpeciesSettings::get().subscriberAddresses();
    if (!subscriberAddresses.isEmpty()) {
        m_ui->bindLabel->setText("Binds:" + subscriberAddresses[0]);

        try {
            m_publisher.connect(subscriberAddresses[0].toStdString().data());
            qDebug() << QString("Connected to %1").arg(subscriberAddresses[0]);
        } catch (const zmq::error_t& e) {
            qDebug() <<  QString("Exception while connecting to %1").arg(subscriberAddresses[0])
                      << e.what();
        }

        connect(&m_timer, &QTimer::timeout, [=](){
            zmq::sendMultipart(m_publisher, "ASSISIbf", "ASSISIbf", "ASSISIbf", "ASSISIbf");
            qDebug() << "Message is sent";
        });
        m_timer.start(1000);
    } else {
        qDebug() << "Don't have an address to publish the data";
    }
}

/*!
 * Destructor.
 */
MainWindow::~MainWindow()
{
    qDebug() << "Destroying the object";
}
