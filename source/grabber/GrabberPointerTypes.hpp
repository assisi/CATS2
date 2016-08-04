#ifndef CATS2_GRABBER_POINTER_TYPES_HPP
#define CATS2_GRABBER_POINTER_TYPES_HPP

#include <QtCore/QSharedPointer>

/*!
 * The alias for the shared pointer to the grabber data.
 */
class GrabberData;
using GrabberDataPtr = QSharedPointer<GrabberData>;

/*!
 * The alias for the shared pointer to the grabber handler.
 */
class GrabberHandler;
using GrabberHandlerPtr = QSharedPointer<GrabberHandler>;

/*!
 * The alias for the stream receiver shared pointer.
 */
class StreamReceiver;
using StreamReceiverPtr = QSharedPointer<StreamReceiver>;

#endif // CATS2_GRABBER_POINTER_TYPES_HPP

