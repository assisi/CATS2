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
    QApplication::setOrganizationName("MOBOTS");
    QApplication::setOrganizationDomain("mobots.epfl.ch");
    QApplication::setApplicationName("CAT2-simple-video-grabber");

    QGst::init(&argc, &argv);
    QApplication app(argc, argv);

    // specify the setup type
    SetupType::Enum setupType = SetupType::MAIN_CAMERA;

    // parse input arguments to initialize the grabber
    if (CommandLineParameters::get().init(argc, argv)) {
        GrabberHandlerPtr grabberHandler;
        if (CommandLineParameters::get().mainCameraDescriptor().isValid()){
            bool needTargetFrameSize = true;
            if (GrabberSettings::get().init(CommandLineParameters::get().configurationFilePath(),
                                            setupType, needTargetFrameSize)) {
                grabberHandler = GrabberHandlerPtr(new GrabberHandler(setupType));
            } else {
                qDebug() << Q_FUNC_INFO << "Grabber settings are not defined";
            }
        } else {
            qDebug() << Q_FUNC_INFO << "Main camera descriptor is ill-defined";
        }
        return app.exec();
    }
}
