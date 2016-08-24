#ifndef CATS2_VIEWER_HANDLER_HPP
#define CATS2_VIEWER_HANDLER_HPP

#include "ViewerPointerTypes.hpp"

#include <CommonPointerTypes.hpp>
#include <SetupType.hpp>

#include <QtCore/QObject>

class ViewerWidget;

/*!
* \brief This class manages the visualization of the video stream.
* Binds together the data and GUI.
 * NOTE : handler classes should managed through smart pointers without using the Qt's mechanism
 * of ownership; thus we set the parent to nullptr in the constructor.
*/
class ViewerHandler : public QObject
{
    Q_OBJECT
public:
    //! Constructor.
    explicit ViewerHandler(SetupType::Enum setupType,
                           TimestampedFrameQueuePtr inputQueue,
                           CoordinatesConversionPtr coordinatesConversion = CoordinatesConversionPtr());
    //! Destructor.
    virtual ~ViewerHandler();

public:
    //! Returns the shared pointer to the data class.
    ViewerDataPtr data() { return m_data; }
    //! Returns the pointer to the viewer widget.
    ViewerWidget* widget() { return m_widget; }

private:
    //! The data class.
    ViewerDataPtr m_data;
    //! The GUI class.
    ViewerWidget* m_widget;
};

#endif // CATS2_VIEWER_HANDLER_HPP
