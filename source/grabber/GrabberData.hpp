#ifndef CATS2_GRABBER_DATA_HPP
#define CATS2_GRABBER_DATA_HPP

#include "GrabberPointerTypes.hpp"

#include <CommonPointerTypes.hpp>
#include <settings/CommandLineParameters.hpp>

#include <QtCore/QObject>

/*!
* \brief This class manages the video reception from one source. It creates and launches a stream receiver in a separated thread.
 * NOTE : this class should be managed through smart pointers without using the Qt's mechanism of ownership; thus we set the
 * parent to nullptr in the constructor.
*/
class GrabberData : public QObject
{
    Q_OBJECT
public:
    //! Constructor. The target frame size defines the expected video frame resolution.
    explicit GrabberData(StreamDescriptor parameters, QSize targetFrameSize, TimestampedFrameQueuePtr outputQueue);
    //! Destructor.
    virtual ~GrabberData();

private slots:
    void onError(QString errorMessage);

private:
    //! The stream receiver created by the grabber.
    StreamReceiverPtr m_streamReceiver;
};

#endif // CATS2_GRABBER_DATA_HPP
