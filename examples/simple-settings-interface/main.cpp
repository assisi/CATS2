#include <settings/CommandLineParameters.hpp>
#include <settings/RobotControlSettings.hpp>
#include <SettingsInterface.hpp>

#include <QtWidgets/QApplication>
#include <QtCore/QSharedPointer>

int main(int argc, char *argv[])
{
    QApplication::setOrganizationName("EPFL-LSRO-Mobots");
    QApplication::setOrganizationDomain("mobots.epfl.ch");
    QApplication::setApplicationName("CATS2-simple-settings-interface");

    QApplication app(argc, argv);

    // parse input arguments to initialize the grabber
    if (CommandLineParameters::get().init(argc, argv)) {
        SettingsInterfacePtr settingsInterface;
        // initialize the robot controller settings
        if ((RobotControlSettings::get().init(CommandLineParameters::get().configurationFilePath()))) {
            settingsInterface = SettingsInterfacePtr(new SettingsInterface());
        }
        return app.exec();
    }
}
