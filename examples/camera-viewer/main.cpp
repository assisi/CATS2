#include "ViewerWindow.hpp"
#include <settings/CommandLineParameters.hpp>

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

        ViewerWindow mainWindow;
        mainWindow.show();
        return app.exec();
    } else {
        qDebug() << Q_FUNC_INFO << "Couldn't find necessary input arguments, finished";
    }

}

