#include <settings/CommandLineParameters.hpp>
#include <settings/CalibrationSettings.hpp>
#include <settings/GrabberSettings.hpp>
#include <settings/ViewerSettings.hpp>
#include <CoordinatesConversion.hpp>
#include <CommonPointerTypes.hpp>
#include <GrabberPointerTypes.hpp>
#include <GrabberData.hpp>
#include <GrabberHandler.hpp>
#include <gui/ViewerWindow.hpp>

#include <QGst/Init>

#include <QtWidgets/QApplication>
#include <QtCore/QDebug>

int main(int argc, char *argv[])
{
    QApplication::setOrganizationName("EPFL-LSRO-Mobots");
    QApplication::setOrganizationDomain("mobots.epfl.ch");
    QApplication::setApplicationName("CATS2-camera-viewer");

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
                    ViewerWindow mainWindow(setupType, grabberHandler->inputQueue(), coordinatesConversion);
                    mainWindow.show();
                    return app.exec();
                }
            } else {
                qDebug() << "Grabber settings are not defined";
            }
        } else {
            qDebug() << "Camera descriptor is ill-defined";
        }
    } else {
        qDebug() << "Couldn't find necessary input arguments, finished";
    }
}

