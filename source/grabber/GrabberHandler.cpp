#include "GrabberHandler.hpp"

#include "GrabberData.hpp"

#include <TimestampedFrame.hpp>

/*!
* Constructor.
*/
GrabberHandler::GrabberHandler(SetupType::Enum setupType) :
    m_queue(new TimestampedFrameQueue(100)),
    m_data(new GrabberData(CommandLineParameters::get().cameraDescriptor(setupType), m_queue), &QObject::deleteLater)
{
}

/*!
* Destructor.
*/
GrabberHandler::~GrabberHandler()
{
}
