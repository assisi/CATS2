#ifndef CATS2_BLOB_DETECTOR_SETTINGS_HPP
#define CATS2_BLOB_DETECTOR_SETTINGS_HPP

#include "TrackingRoutineSettings.hpp"

/*!
 * The actually data stored in the settings. It's separated in a class to be easily trasferable
 * to the corresponding tracking routine.
 */
class BlobDetectorSettingsData
{
public:
    //! Constructor.
    BlobDetectorSettingsData() :
        _numberOfAgents(0),
        _minBlobSizePx(15),
        _qualityLevel(0.01),
        _minDistance(20.0),
        _blockSize(5),
        _useHarrisDetector(false),
        _k(0.04)
    {}

public:
    //! Return the number of agents to be tracked.
    int numberOfAgents() const { return _numberOfAgents; }
    //! Set the number of agents.
    void setNumberOfAgents(int numberOfAgents) { _numberOfAgents = numberOfAgents; }
    //! Returns the value of _minBlobSizePx
    int minBlobSizePx() const { return _minBlobSizePx; }
    void setMinBlobSizePx(int minBlobSizePx) { _minBlobSizePx = minBlobSizePx; }

    double qualityLevel() const { return _qualityLevel; }
    void setQualityLevel(double qualityLevel) { _qualityLevel = qualityLevel; }

    double minDistance() const { return _minDistance; }
    void setMinDistance(double minDistance) { _minDistance = minDistance; }

    int blockSize() const { return _blockSize; }
    void setBlockSize(int blockSize) { _blockSize = blockSize; }

    bool useHarrisDetector() const { return _useHarrisDetector; }
    void setUseHarrisDetector(bool useHarrisDetector) { _useHarrisDetector = useHarrisDetector; }

    double k() const { return _k; }
    void setK(double k) { _k = k; }

protected:
    //! Number of agents to track.
    int _numberOfAgents;
    //! All the blobs that are smaller than this size will not be taken into account
    //! by the fish search algorithm.
    int _minBlobSizePx; // in [px]
    //!
    double _qualityLevel;
    //!
    double _minDistance;
    //!
    int _blockSize;
    //!
    bool _useHarrisDetector;
    //!
    double _k;
};

/*!
 * The settings for the blob detector routine.
 */
class BlobDetectorSettings : public TrackingRoutineSettings
{
public:
    //! Constructor.
    BlobDetectorSettings(QString settingPathPrefix);

public:
    //! Initialization of the parameters for this specific method of tracking.
    //! Returns true if the parameters are considered valid.
    virtual bool init(QString configurationFileName) override;

    //! Provides a copy of the settings data.
    BlobDetectorSettingsData data() { return _data; }

private:
    //! The settings data.
    BlobDetectorSettingsData _data;
};

#endif // CATS2_BLOB_DETECTOR_SETTINGS_HPP
