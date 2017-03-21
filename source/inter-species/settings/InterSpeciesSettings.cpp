#include "InterSpeciesSettings.hpp"

#include "settings/ReadSettingsHelper.hpp"


/*!
 * The singleton getter. Provides an instance of the settings.
 */
InterSpeciesSettings& InterSpeciesSettings::get()
{
    static InterSpeciesSettings instance;
    return instance;
}

/*!
 * Initializes the parameters from the configuration file.
 */
bool InterSpeciesSettings::init(QString configurationFileName, bool needPublisherAddress)
{
    bool settingsAccepted = true;

    ReadSettingsHelper settings(configurationFileName);

    // read the target image size
    std::string address = "";
    settings.readVariable(QString("interSpecies/publisherAddress"), address, address);
    m_publisherAddress = QString::fromStdString(address);
    if (m_publisherAddress.isNull())
        qDebug() << "The publisher address is not set in the configuration file";

    settingsAccepted = ((!m_publisherAddress.isNull()) || (!needPublisherAddress));

    return settingsAccepted;
}
