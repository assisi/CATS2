#ifndef CATS2_STREAM_DESCRIPTOR_HPP
#define CATS2_STREAM_DESCRIPTOR_HPP

#include <QtCore/QMap>

/*!
 * \brief The type of the incoming video stream.
 */
enum class StreamType
{
    VIDEO_4_LINUX,
    LOCAL_VIDEO_FILE,
    LOCAL_IMAGE_FILE, // still image (for the debug purposes)
    UNDEFINED
    // NOTE : to be extended
};

/*!
 * Stores the input stream type and parameters.
 */
class StreamDescriptor
{
public:
    //! Constructor.
    explicit StreamDescriptor(StreamType streamType = StreamType::UNDEFINED,
                              QString parameters = ""):
        m_streamType(streamType),
        m_parameters(parameters)
    {}

    //! Convenience constructor, takes the stream type as as string.
    StreamDescriptor(QString streamType, QString parameters) :
        m_streamType(StreamType::UNDEFINED)
    {
        if (m_streamTypeByName.contains(streamType)){
            m_streamType = m_streamTypeByName[streamType];
            m_parameters = parameters;
        }
    }

    //! Getter for the stream type.
    StreamType streamType() const { return m_streamType; }
    //! Getter for the parameters.
    QString parameters() const { return m_parameters; }
    //! Checks that the stream descriptor is valid.
    bool isValid() const { return (m_streamType != StreamType::UNDEFINED); }

    //! Checks that the provided string is a valid stream type.
    static bool isValidStreamType(QString streamType)
    {
        return m_streamTypeByName.contains(streamType);
    }

private:
    //! The type of the incoming video stream.
    StreamType m_streamType;
    //! Additional parameters of the stream (e.g. device id for the camera,
    //! input file name, etc.).
    QString m_parameters;

    //! The map to translate the string stream type to the corresponding enum.
    static const QMap<QString, StreamType> m_streamTypeByName;
};

#endif // CATS2_STREAM_DESCRIPTOR_HPP
