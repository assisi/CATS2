#ifndef CATS2_VIEWER_POINTER_TYPES_HPP
#define CATS2_VIEWER_POINTER_TYPES_HPP

#include <QtCore/QSharedPointer>

/*!
 * The alias for the shared pointer to a queue of the viewer handler.
 */
class ViewerHandler;
using ViewerHandlerPtr = QSharedPointer<ViewerHandler>;

/*!
 * The alias for the shared pointer to viewer data.
 */
class ViewerData;
using ViewerDataPtr = QSharedPointer<ViewerData>;

/*!
 * The alias for the shared pointer to frame convertor.
 */
class FrameConvertor;
using FrameConvertorPtr = QSharedPointer<FrameConvertor>;

#endif // CATS2_VIEWER_POINTER_TYPES_HPP

