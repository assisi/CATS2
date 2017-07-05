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
bool InterSpeciesSettings::init(QString configurationFileName,
                                bool needPublisherAddress,
                                bool needSubscriberAddress)
{
    bool settingsAccepted = true;

    ReadSettingsHelper settings(configurationFileName);

    // read the publisher address
    std::string address = "";
    settings.readVariable(QString("interSpecies/publisherAddress"), address, address);
    m_publisherAddress = QString::fromStdString(address);
    if (m_publisherAddress.isNull())
        qDebug() << "The publisher address is not set in the configuration file";

    settingsAccepted = ((!m_publisherAddress.isNull()) || (!needPublisherAddress));

    // read the subscriber address
    address = "";
    settings.readVariable(QString("interSpecies/subscriberAddresses"), address, address);
    m_subscriberAddresses = QString::fromStdString(address).split(";");
    if (m_subscriberAddresses.isEmpty())
        qDebug() << "The subscriber address is not set in the configuration file";

    settingsAccepted = ((!m_subscriberAddresses.isEmpty()) || (!needSubscriberAddress));

    return settingsAccepted;
}
