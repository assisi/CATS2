#include <QGst/Init>

#include <QtWidgets/QApplication>
#include <QtCore/QDebug>

#include "MainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("EPFL-LSRO-Mobots");
    QCoreApplication::setOrganizationDomain("mobots.epfl.ch");
    QCoreApplication::setApplicationName("CameraCalibrationHelper");

    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}

