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
 * The alias for the shared pointer to the tracking routine settings class.
 */
class TrackingRoutineSettings;
using TrackingRoutineSettingsPtr = QSharedPointer<TrackingRoutineSettings>;

/*!
 * The alias for the shared pointer to the tracking data.
 */
class TrackingData;
using TrackingDataPtr = QSharedPointer<TrackingData>;

/*!
 * The alias for the shared pointer to the tracking setup.
 */
class TrackingSetup;
using TrackingSetupPtr = QSharedPointer<TrackingSetup>;

/*!
 * The alias for the shared pointer to the tracking data manager.
 */
class TrackingDataManager;
using TrackingDataManagerPtr = QSharedPointer<TrackingDataManager>;

#endif // CATS2_TRACKER_POINTER_TYPES_HPP

