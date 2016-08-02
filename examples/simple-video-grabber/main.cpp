#include <GrabberData.hpp>
#include <settings/CommandLineParameters.hpp>
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

    // parse input arguments to initialize the grabber
    if (CommandLineParameters::get().init(argc, argv)) {
        TimestampedFrameQueuePtr queuePtr = TimestampedFrameQueuePtr(new TimestampedFrameQueue(100));
        QSharedPointer<GrabberData> grabber;
        if (CommandLineParameters::get().mainCameraDescriptor().isValid()){
            grabber = QSharedPointer<GrabberData>(new GrabberData(CommandLineParameters::get().mainCameraDescriptor(), queuePtr), &QObject::deleteLater);
        }
        return app.exec();
    }
}
