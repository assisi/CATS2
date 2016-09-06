#ifndef CATS2_BLOB_DETECTOR_SETTINGS_HPP
#define CATS2_BLOB_DETECTOR_SETTINGS_HPP

#include "TrackingRoutineSettings.hpp"

/*!
 * The actual data stored in the settings. It's separated in a class to be easily trasferable
 * to the corresponding tracking routine.
 */
class BlobDetectorSettingsData
{
public:
    //! Constructor.
    BlobDetectorSettingsData() :
        m_numberOfAgents(0),
        m_minBlobSizePx(15),
        m_qualityLevel(0.01),
        m_minDistance(20.0),
        m_blockSize(5),
        m_useHarrisDetector(false),
        m_k(0.04)
    {}

public:
    //! Return the number of agents to be tracked.
    int numberOfAgents() const { return m_numberOfAgents; }
    //! Set the number of agents.
    void setNumberOfAgents(int numberOfAgents) { m_numberOfAgents = numberOfAgents; }
    //! Returns the value of _minBlobSizePx
    int minBlobSizePx() const { return m_minBlobSizePx; }
    void setMinBlobSizePx(int minBlobSizePx) { m_minBlobSizePx = minBlobSizePx; }

    double qualityLevel() const { return m_qualityLevel; }
    void setQualityLevel(double qualityLevel) { m_qualityLevel = qualityLevel; }

    double minDistance() const { return m_minDistance; }
    void setMinDistance(double minDistance) { m_minDistance = minDistance; }

    int blockSize() const { return m_blockSize; }
    void setBlockSize(int blockSize) { m_blockSize = blockSize; }

    bool useHarrisDetector() const { return m_useHarrisDetector; }
    void setUseHarrisDetector(bool useHarrisDetector) { m_useHarrisDetector = useHarrisDetector; }

    double k() const { return m_k; }
    void setK(double k) { m_k = k; }

protected:
    //! Number of agents to track.
    int m_numberOfAgents;
    //! All the blobs that are smaller than this size will not be taken into account
    //! by the fish search algorithm.
    int m_minBlobSizePx; // in [px]
    //!
    double m_qualityLevel;
    //!
    double m_minDistance;
    //!
    int m_blockSize;
    //!
    bool m_useHarrisDetector;
    //!
    double m_k;
};

/*!
 * The settings for the blob detector routine.
 */
class BlobDetectorSettings : public TrackingRoutineSettings
{
public:
    //! Constructor.
    BlobDetectorSettings(SetupType::Enum setupType);

public:
    //! Initialization of the parameters for this specific method of tracking.
    //! Returns true if the parameters are considered valid.
    virtual bool init(QString configurationFileName) override;

    //! Provides a copy of the settings data.
    BlobDetectorSettingsData data() { return m_data; }

private:
    //! The settings data.
    BlobDetectorSettingsData m_data;
};

#endif // CATS2_BLOB_DETECTOR_SETTINGS_HPP
