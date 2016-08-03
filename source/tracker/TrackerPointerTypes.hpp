#ifndef CATS2_TRACKER_POINTER_TYPES_HPP
#define CATS2_TRACKER_POINTER_TYPES_HPP

#include <QtCore/QSharedPointer>

/*!
 * The alias for the shared pointer to tracking handler.
 */
class TrackingHandler;
using TrackingHandlerPtr = QSharedPointer<TrackingHandler>;

#endif // CATS2_TRACKER_POINTER_TYPES_HPP

