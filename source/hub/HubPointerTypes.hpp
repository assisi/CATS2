#ifndef CATS2_HUB_POINTER_TYPES_HPP
#define CATS2_HUB_POINTER_TYPES_HPP

#include <QtCore/QSharedPointer>

/*!
 * The alias for the shared pointer to multiplicator.
 */
class Multiplicator;
using MultiplicatorPtr = QSharedPointer<Multiplicator>;

/*!
 * The alias for the shared pointer to the queue hub.
 */
class QueueHub;
using QueueHubPtr = QSharedPointer<QueueHub>;

#endif // CATS2_HUB_POINTER_TYPES_HPP

