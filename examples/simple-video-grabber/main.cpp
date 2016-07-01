#include "VideoGrabber.hpp"
#include "VideoGrabberSettings.hpp"

#include <QGst/Init>

#include <QApplication>

int main(int argc, char *argv[])
{
    QGst::init(&argc, &argv);
    QApplication app(argc, argv);

    // parse input arguments to initialize the grabber
    if (VideoGrabberSettings::get().init(argc, argv)) {
        TimestampedFrameQueuePtr queuePtr = QSharedPointer<TimestampedFrameQueue>(new TimestampedFrameQueue(100));
        VideoGrabber grabber;
        if (VideoGrabberSettings::get().mainCameraDescriptor().isValid()){
            grabber.addStreamReceiver(VideoGrabberSettings::get().mainCameraDescriptor(), queuePtr);
        }
        return app.exec();
    }
}
