#include "ViewerData.hpp"
#include "FrameConvertor.hpp"

#include <QtCore/QThread>
#include <QtGui/QImage>

/*!
 * Constructor.
 */
ViewerData::ViewerData(TimestampedFrameQueuePtr inputQueue) : QObject(nullptr)
{
    // launch the incoming frames conversion in separated thread
    QThread* thread = new QThread;
    m_convertor = QSharedPointer<FrameConvertor>(new FrameConvertor(inputQueue), &QObject::deleteLater); // delete later is used for security as multithreaded
                                                                                                        // signals and slots might result is crashes when a
                                                                                                        // a sender is deleted before a signal is received for instance
    m_convertor->moveToThread(thread);

    connect(thread, &QThread::started, m_convertor.data(), &FrameConvertor::process);
    connect(m_convertor.data(), &FrameConvertor::finished, thread, &QThread::quit);
    connect(m_convertor.data(), &FrameConvertor::finished, [=]() { m_convertor.clear(); });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(m_convertor.data(), &FrameConvertor::newFrame, this, &ViewerData::newFrame);

    thread->start();
}

/*!
 * Destructor.
 */
ViewerData::~ViewerData()
{
    m_convertor->stop();
}

