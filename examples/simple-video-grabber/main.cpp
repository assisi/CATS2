#include <GrabberData.hpp>
#include <GrabberHandler.hpp>
#include <settings/CommandLineParameters.hpp>
#include <settings/GrabberSettings.hpp>
#include <TimestampedFrame.hpp>
#include <CommonPointerTypes.hpp>

#include <QGst/Init>

#include <QtWidgets/QApplication>
#include <QtCore/QSharedPointer>

int main(int argc, char *argv[])
{
    QApplication::setOrganizationName("EPFL-LSRO-Mobots");
    QApplication::setOrganizationDomain("mobots.epfl.ch");
    QApplication::setApplicationName("CATS2-simple-video-grabber");

    QGst::init(nullptr, nullptr);
    QApplication app(argc, argv);

    // specify the setup type
    SetupType::Enum setupType = SetupType::MAIN_CAMERA;

    // parse input arguments to initialize the grabber
    if (CommandLineParameters::get().init(argc, argv)) {
        GrabberHandlerPtr grabberHandler;
        if (CommandLineParameters::get().cameraDescriptor(setupType).isValid()){
            bool needTargetFrameSize = true;
            if (GrabberSettings::get().init(CommandLineParameters::get().configurationFilePath(),
                                            setupType, needTargetFrameSize)) {
                grabberHandler = GrabberHandlerPtr(new GrabberHandler(setupType));
            } else {
                qDebug() << "Grabber settings are not defined";
            }
        } else {
            qDebug() << "Main camera descriptor is ill-defined";
        }
        return app.exec();
    }
}
