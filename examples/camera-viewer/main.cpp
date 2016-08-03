#include <settings/CommandLineParameters.hpp>
#include <settings/CalibrationSettings.hpp>
#include <CoordinatesConversion.hpp>
#include <CommonPointerTypes.hpp>
#include <GrabberPointerTypes.hpp>
#include <GrabberData.hpp>
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

    // parse input arguments to initialize the grabber
    if (CommandLineParameters::get().init(argc, argv)) {
        // check that the calibration settings are valid
        if (CalibrationSettings::get().init(CommandLineParameters::get().configurationFilePath(), SetupType::MAIN_CAMERA)) {
            // create the camera calibration
            CoordinatesConversionPtr coordinatesConversion = CoordinatesConversionPtr(new CoordinatesConversion(CalibrationSettings::get().calibrationFilePath(SetupType::MAIN_CAMERA)));

            // exchange queue between the grabber and the viewer
            TimestampedFrameQueuePtr queuePtr = TimestampedFrameQueuePtr(new TimestampedFrameQueue(100));

            // create the grabber
            GrabberDataPtr grabber;
            if (CommandLineParameters::get().mainCameraDescriptor().isValid()){
                grabber = GrabberDataPtr(new GrabberData(CommandLineParameters::get().mainCameraDescriptor(), queuePtr), &QObject::deleteLater);
            } else {
                qDebug() << Q_FUNC_INFO << "Main camera descriptor is ill-defined";
            }

            ViewerWindow mainWindow(queuePtr, coordinatesConversion);
            mainWindow.show();
            return app.exec();

        } else {
            qDebug() << Q_FUNC_INFO << "Couldn't setup calibration settings, finished";
        }
    } else {
        qDebug() << Q_FUNC_INFO << "Couldn't find necessary input arguments, finished";
    }

}

