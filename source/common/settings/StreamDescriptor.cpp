#include "StreamDescriptor.hpp"

/*!
 * The map to translate the string stream type to the corresponding enum.
 */
const QMap<QString, StreamType>
StreamDescriptor::m_streamTypeByName = {{"v4l", StreamType::VIDEO_4_LINUX},
                                        {"vf", StreamType::LOCAL_VIDEO_FILE},
                                        {"if", StreamType::LOCAL_IMAGE_FILE}};
