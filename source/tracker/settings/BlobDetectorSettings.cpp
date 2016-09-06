#include "BlobDetectorSettings.hpp"

#include <settings/ReadSettingsHelper.hpp>

/*!
 * Constructor.
 */
BlobDetectorSettings::BlobDetectorSettings(SetupType::Enum setupType) :
    TrackingRoutineSettings(setupType)
{
    m_trackingRoutineType = TrackingRoutineType::BLOB_DETECTOR;
}

/*!
 * Initialization of the parameters for this specific method of tracking.
 */
bool BlobDetectorSettings::init(QString configurationFileName)
{
    // read the settings
    ReadSettingsHelper settings(configurationFileName);

    // we pass the variable as defaulf value
    {
        int value;
        settings.readVariable(QString("%1/tracking/numberOfAgents").arg(m_settingPathPrefix), value, m_data.numberOfAgents());
        m_data.setNumberOfAgents(value);

        settings.readVariable(QString("%1/tracking/blobDetector/minBlobSizePx").arg(m_settingPathPrefix), value, m_data.minBlobSizePx());
        m_data.setMinBlobSizePx(value);

        settings.readVariable(QString("%1/tracking/blobDetector/blockSize").arg(m_settingPathPrefix), value, m_data.blockSize());
        m_data.setBlockSize(value);
    }

    {
        double value;
        settings.readVariable(QString("%1/tracking/blobDetector/qualityLevel").arg(m_settingPathPrefix), value, m_data.qualityLevel());
        m_data.setQualityLevel(value);
        settings.readVariable(QString("%1/tracking/blobDetector/minDistance").arg(m_settingPathPrefix), value, m_data.minDistance());
        m_data.setMinDistance(value);
        settings.readVariable(QString("%1/tracking/blobDetector/k").arg(m_settingPathPrefix), value, m_data.k());
        m_data.setK(value);
    }
    bool value;
    settings.readVariable(QString("%1/tracking/blobDetector/useHarrisDetector").arg(m_settingPathPrefix), value, m_data.useHarrisDetector());
    m_data.setUseHarrisDetector(value);

    return (m_data.numberOfAgents() > 0);
}


