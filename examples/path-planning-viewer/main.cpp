#include "MainWindow.hpp"

#include <settings/CommandLineParameters.hpp>
#include <settings/CalibrationSettings.hpp>
#include <settings/GrabberSettings.hpp>
#include <settings/ViewerSettings.hpp>
#include <settings/RobotControlSettings.hpp>
#include <CoordinatesConversion.hpp>
#include <CommonPointerTypes.hpp>
#include <GrabberPointerTypes.hpp>
#include <GrabberData.hpp>
#include <GrabberHandler.hpp>

#include <QGst/Init>

#include <QtWidgets/QApplication>
#include <QtCore/QDebug>


int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("EPFL-LSRO-Mobots");
    QCoreApplication::setOrganizationDomain("mobots.epfl.ch");
    QCoreApplication::setApplicationName("PathPlanningViewer");

    QGst::init(nullptr, nullptr);
    QApplication app(argc, argv);

    // specify the setup type
    SetupType::Enum setupType = SetupType::MAIN_CAMERA;

    // parse input arguments to initialize the settings
    if (CommandLineParameters::get().init(argc, argv, true, false, false)) {
        CoordinatesConversionPtr coordinatesConversion;
        // check that the calibration settings are valid
        if (CalibrationSettings::get().init(CommandLineParameters::get().configurationFilePath(), setupType)) {
            // create the camera calibration
            coordinatesConversion = CoordinatesConversionPtr(new CoordinatesConversion(CalibrationSettings::get().calibrationFilePath(setupType),
                                                                                       CalibrationSettings::get().frameSize(setupType)));
        }
        GrabberHandlerPtr grabberHandler;
        if (CommandLineParameters::get().cameraDescriptor(setupType).isValid()){
            bool needTargetFrameSize = false;
            if (GrabberSettings::get().init(CommandLineParameters::get().configurationFilePath(),
                                            setupType, needTargetFrameSize)) {
                grabberHandler = GrabberHandlerPtr(new GrabberHandler(setupType));
                if (ViewerSettings::get().init(CommandLineParameters::get().configurationFilePath(), setupType)){
                    if (RobotControlSettings::get().init(CommandLineParameters::get().configurationFilePath())) {
                        MainWindow mainWindow(setupType, grabberHandler->inputQueue(), coordinatesConversion);
                        mainWindow.show();
                        return app.exec();
                    }
                }
            } else {
                qDebug() << Q_FUNC_INFO << "Grabber settings are not defined";
            }
        } else {
            qDebug() << Q_FUNC_INFO << "Camera descriptor is ill-defined";
        }

    } else {
        qDebug() << Q_FUNC_INFO << "Couldn't find necessary input arguments, finished";
    }
}

