#ifndef CATS2_GRABBER_HANDLER_HPP
#define CATS2_GRABBER_HANDLER_HPP

#include "GrabberPointerTypes.hpp"

#include <CommonPointerTypes.hpp>
#include <SetupType.hpp>

/*!
* \brief This class manages the creation on the grabber data and the corresponding queue.
* NOTE : handler classes should managed through smart pointers without using the Qt's mechanism
* of ownership; thus we set the parent to nullptr in the constructor.
*/
class GrabberHandler
{
public:
    //! Constructor.
    explicit GrabberHandler(SetupType::Enum setupType);
    //! Destructor.
    virtual ~GrabberHandler() final;

public:
    //! Returns the shared pointer to the data.
    GrabberDataPtr data() { return m_data; }
    //! Returns the shared pointer to the queue.
    TimestampedFrameQueuePtr inputQueue() { return m_queue; }

private:
    //! Input queue for the grabber.
    TimestampedFrameQueuePtr m_queue;
    //! The data class.
    GrabberDataPtr m_data;
};

#endif // CATS2_GRABBER_HANDLER_HPP
