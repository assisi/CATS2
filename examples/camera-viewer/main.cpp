#include "MainWindow.hpp"
#include <VideoGrabberSettings.hpp>

#include <QGst/Init>

#include <QtWidgets/QApplication>
#include <QtCore/QDebug>

int main(int argc, char *argv[])
{
    QGst::init(&argc, &argv);
    QApplication app(argc, argv);

    // parse input arguments to initialize the grabber
    if (VideoGrabberSettings::get().init(argc, argv)) {

        MainWindow mainWindow;
        mainWindow.show();
        return app.exec();
    } else {
        qDebug() << "Couldn't find necessary input arguments, finished";
    }

}

