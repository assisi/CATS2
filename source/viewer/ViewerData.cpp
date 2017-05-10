#include "ViewerData.hpp"
#include "FrameConvertor.hpp"

#include <QtCore/QThread>
#include <QtCore/QDebug>
#include <QtGui/QImage>

/*!
 * Constructor.
 */
ViewerData::ViewerData(TimestampedFrameQueuePtr inputQueue,
                       CoordinatesConversionPtr coordinatesConversion) :
    QObject(nullptr),
    m_coordinatesConversion(coordinatesConversion)
{
    // launch the incoming frames conversion in separated thread
    QThread* thread = new QThread;
    m_frameConvertor = FrameConvertorPtr(new FrameConvertor(inputQueue),
                                         &FrameConvertor::deleteLater); // delete later is used for security as multithreaded
                                                                          // signals and slots might result is crashes when a
                                                                          // a sender is deleted before a signal is received for instance
    m_frameConvertor->moveToThread(thread);

    connect(thread, &QThread::started, m_frameConvertor.data(), &FrameConvertor::process);
    connect(m_frameConvertor.data(), &FrameConvertor::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(m_frameConvertor.data(), &FrameConvertor::newFrame, this, &ViewerData::newFrame);

    thread->start();
}

/*!
 * Destructor.
 */
ViewerData::~ViewerData()
{
    qDebug() << "Destroying the object";
    m_frameConvertor->stop();
}

