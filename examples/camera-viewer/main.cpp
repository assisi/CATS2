#include <settings/CommandLineParameters.hpp>
#include <settings/CalibrationSettings.hpp>
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
    QApplication::setOrganizationName("MOBOTS");
    QApplication::setOrganizationDomain("mobots.epfl.ch");
    QApplication::setApplicationName("CAT2-camera-viewer");

    QGst::init(&argc, &argv);
    QApplication app(argc, argv);

    // specify the setup type
    SetupType::Enum setupType = SetupType::CAMERA_BELOW;

    // parse input arguments to initialize the settings
    if (CommandLineParameters::get().init(argc, argv, true, false, false)) {
        CoordinatesConversionPtr coordinatesConversion;
        // check that the calibration settings are valid
        if (CalibrationSettings::get().init(CommandLineParameters::get().configurationFilePath(), setupType)) {
            // create the camera calibration
            coordinatesConversion = CoordinatesConversionPtr(new CoordinatesConversion(CalibrationSettings::get().calibrationFilePath(setupType)));
        }
        GrabberHandlerPtr grabberHandler;
        if (CommandLineParameters::get().cameraDescriptor(setupType).isValid()){
            grabberHandler = GrabberHandlerPtr(new GrabberHandler(setupType));
        } else {
            qDebug() << Q_FUNC_INFO << "Camera descriptor is ill-defined";
        }

        ViewerWindow mainWindow(grabberHandler->inputQueue(), coordinatesConversion);
        mainWindow.show();
        return app.exec();
    } else {
        qDebug() << Q_FUNC_INFO << "Couldn't find necessary input arguments, finished";
    }

}

