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
    QApplication::setOrganizationName("MOBOTS");
    QApplication::setOrganizationDomain("mobots.epfl.ch");
    QApplication::setApplicationName("CAT2-simple-tracker");

    QGst::init(&argc, &argv);
    QApplication app(argc, argv);

    // specify the setup type
    SetupType::Enum setupType = SetupType::CAMERA_BELOW;

    // parse input arguments to initialize the settings
    if (CommandLineParameters::get().init(argc, argv, true, false, false)) {
        if (CommandLineParameters::get().cameraDescriptor(setupType).isValid()) {
            bool needCalibration = false;
            if (TrackingSetupSettings::init(setupType, needCalibration)) {
                MainWindow mainWindow(setupType);
                mainWindow.show();
                return app.exec();
            } else {
                qDebug() << Q_FUNC_INFO << "Couldn't setup the camera, finished";
            }
        } else {
            qDebug() << Q_FUNC_INFO << "Invalid camera descriptor" << CommandLineParameters::get().cameraDescriptor(setupType).parameters();
        }
    } else {
        qDebug() << Q_FUNC_INFO << "Couldn't find necessary input arguments, finished";
    }
}

