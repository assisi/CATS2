#include "MainWindow.hpp"

#include <settings/TrackingSetupSettings.hpp>
#include <TrackerPointerTypes.hpp>
#include <TrackingSetup.hpp>
#include <settings/CommandLineParameters.hpp>

#include <QGst/Init>

#include <QtWidgets/QApplication>
#include <QtCore/QDebug>

int main(int argc, char *argv[])
{
    QApplication::setOrganizationName("EPFL-LSRO-Mobots");
    QApplication::setOrganizationDomain("mobots.epfl.ch");
    QApplication::setApplicationName("CATS2-simple-tracker");

    QGst::init(nullptr, nullptr);
    QApplication app(argc, argv);

    // specify the setup type
    SetupType::Enum setupType = SetupType::MAIN_CAMERA;

    // parse input arguments to initialize the settings
    if (CommandLineParameters::get().init(argc, argv, true, false, false)) {
        if (CommandLineParameters::get().cameraDescriptor(setupType).isValid()) {
            bool needCalibration = false;
            if (TrackingSetupSettings::init(CommandLineParameters::get().configurationFilePath(),
                                            setupType, needCalibration)) {
                MainWindow mainWindow(setupType);
                mainWindow.show();
                return app.exec();
            } else {
                qDebug() << "Couldn't setup the camera, finished";
            }
        } else {
            qDebug() << "Invalid camera descriptor" << CommandLineParameters::get().cameraDescriptor(setupType).parameters();
        }
    } else {
        qDebug() << "Couldn't find necessary input arguments, finished";
    }
}

