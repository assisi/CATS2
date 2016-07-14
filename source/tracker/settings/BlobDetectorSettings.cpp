#include "BlobDetectorSettings.hpp"

#include <ReadSettingsHelper.hpp>

/*!
 * Constructor.
 */
BlobDetectorSettings::BlobDetectorSettings(QString settingPathPrefix) :
    TrackingRoutineSettings(settingPathPrefix)
{
    _trackingRoutineType = TrackingRoutineType::BLOB_DETECTOR;
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
        settings.readVariable(QString("%1/tracking/numberOfAgents").arg(_settingPathPrefix), value, _data.numberOfAgents());
        _data.setNumberOfAgents(value);

        settings.readVariable(QString("%1/tracking/blobDetector/minBlobSizePx").arg(_settingPathPrefix), value, _data.minBlobSizePx());
        _data.setMinBlobSizePx(value);

        settings.readVariable(QString("%1/tracking/blobDetector/blockSize").arg(_settingPathPrefix), value, _data.blockSize());
        _data.setBlockSize(value);
    }

    {
        double value;
        settings.readVariable(QString("%1/tracking/blobDetector/qualityLevel").arg(_settingPathPrefix), value, _data.qualityLevel());
        _data.setQualityLevel(value);
        settings.readVariable(QString("%1/tracking/blobDetector/minDistance").arg(_settingPathPrefix), value, _data.minDistance());
        _data.setMinDistance(value);
        settings.readVariable(QString("%1/tracking/blobDetector/k").arg(_settingPathPrefix), value, _data.k());
        _data.setK(value);
    }
    bool value;
    settings.readVariable(QString("%1/tracking/blobDetector/useHarrisDetector").arg(_settingPathPrefix), value, _data.useHarrisDetector());
    _data.setUseHarrisDetector(value);

    return (_data.numberOfAgents() > 0);
}


