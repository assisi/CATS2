#include "ui_MainWindow.h"
#include "MainWindow.hpp"

#include <InterSpeciesDataManager.hpp>
#include <settings/InterSpeciesSettings.hpp>

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

    QString publisherAddress = InterSpeciesSettings::get().publisherAddress();
    QString subscriberAddress = InterSpeciesSettings::get().subscriberAddress();
    m_ui->subscriberLabel->setText("Subscriber:" + subscriberAddress);
    m_ui->publisherLabel->setText("Publisher:" + publisherAddress);

    m_communicationManager = InterSpeciesDataManagerPtr(new InterSpeciesDataManager(publisherAddress,
                                                                                    subscriberAddress));
}

/*!
 * Destructor.
 */
MainWindow::~MainWindow()
{
    qDebug() << "Destroying the object";
}
