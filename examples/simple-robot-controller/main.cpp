#include "MainWindow.hpp"

#include <settings/CommandLineParameters.hpp>
#include <settings/RobotControlSettings.hpp>

#include <QGst/Init>

#include <QtWidgets/QApplication>
#include <QtCore/QDebug>

int main(int argc, char *argv[])
{
    QApplication::setOrganizationName("EPFL-LSRO-Mobots");
    QApplication::setOrganizationDomain("mobots.epfl.ch");
    QApplication::setApplicationName("CATS2-simple-robot-controller");

    QApplication app(argc, argv);

    // parse input arguments to initialize the settings
    if (CommandLineParameters::get().init(argc, argv, true, false, false)) {
        if (RobotControlSettings::get().init(CommandLineParameters::get().configurationFilePath())) {
                MainWindow mainWindow;
                mainWindow.show();
                return app.exec();
        } else {
            qDebug() << Q_FUNC_INFO << "Couldn't setup the robot control, finished";
        }
    } else {
        qDebug() << Q_FUNC_INFO << "Couldn't find necessary input arguments, finished";
    }
}

