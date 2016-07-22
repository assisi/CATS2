#include <VideoGrabber.hpp>
#include <settings/CommandLineParameters.hpp>
#include <TimestampedFrame.hpp>
#include <CommonTypes.hpp>

#include <QGst/Init>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setOrganizationName("MOBOTS");
    QApplication::setOrganizationDomain("mobots.epfl.ch");
    QApplication::setApplicationName("CAT2-simple-video-grabber");

    QGst::init(&argc, &argv);
    QApplication app(argc, argv);

    // parse input arguments to initialize the grabber
    if (CommandLineParameters::get().init(argc, argv)) {
        TimestampedFrameQueuePtr queuePtr = TimestampedFrameQueuePtr(new TimestampedFrameQueue(100));
        VideoGrabber grabber;
        if (CommandLineParameters::get().mainCameraDescriptor().isValid()){
            grabber.addStreamReceiver(CommandLineParameters::get().mainCameraDescriptor(), queuePtr);
        }
        return app.exec();
    }
}
