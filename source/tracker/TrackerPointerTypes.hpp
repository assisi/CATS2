#ifndef CATS2_TRACKER_POINTER_TYPES_HPP
#define CATS2_TRACKER_POINTER_TYPES_HPP

#include <QtCore/QSharedPointer>

/*!
 * The alias for the shared pointer to tracking handler.
 */
class TrackingHandler;
using TrackingHandlerPtr = QSharedPointer<TrackingHandler>;

/*!
 * The alias for the shared pointer to tracking routine.
 */
class TrackingRoutine;
using TrackingRoutinePtr = QSharedPointer<TrackingRoutine>;

/*!
 * The alias for the pointer to the tracking routine settings class.
 */
class TrackingRoutineSettings;
using TrackingRoutineSettingsPtr = QSharedPointer<TrackingRoutineSettings>;

/*!
 * The alias for the pointer to the tracking data.
 */
class TrackingData;
using TrackingDataPtr = QSharedPointer<TrackingData>;

#endif // CATS2_TRACKER_POINTER_TYPES_HPP

