#ifndef CATS2_COMMON_POINTER_TYPES_HPP
#define CATS2_COMMON_POINTER_TYPES_HPP

#include <QtCore/QSharedPointer>

/*!
 * The alias for the stream descriptors list.
 */
class StreamDescriptor;
using StreamDescriptorList = QList<StreamDescriptor>;

/*!
 * The alias for the shared pointer to CoordinatesConversion.
 */
class CoordinatesConversion;
using CoordinatesConversionPtr = QSharedPointer<CoordinatesConversion>;

/*!
 * The alias for the shared pointer to a queue of the timestamped frames.
 */
class TimestampedFrameQueue;
using TimestampedFrameQueuePtr = QSharedPointer<TimestampedFrameQueue>;

/*!
 * The alias for the shared pointer to camera calibration.
 */
class CameraCalibration;
using CameraCalibrationPtr = QSharedPointer<CameraCalibration>;

#endif // CATS2_COMMON_POINTER_TYPES_HPP

