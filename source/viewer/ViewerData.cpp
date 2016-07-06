#include "ViewerData.hpp"
#include "FrameConvertor.hpp"

#include <QtCore/QThread>
#include <QtGui/QImage>

/*!
 * Constructor.
 */
ViewerData::ViewerData(TimestampedFrameQueuePtr inputQueue, QObject *parent) : QObject(parent)
{
    // launch the incoming frames conversion in separated thread
    QThread* thread = new QThread;
    _convertor = QSharedPointer<FrameConvertor>(new FrameConvertor(inputQueue), &QObject::deleteLater); // delete later is used for security as multithreaded
                                                                                                        // signals and slots might result is crashes when a
                                                                                                        // a sender is deleted before a signal is received for instance
    _convertor.data()->moveToThread(thread);

    connect(thread, &QThread::started, _convertor.data(), &FrameConvertor::process);
    connect(_convertor.data(), &FrameConvertor::finished, thread, &QThread::quit);
    connect(_convertor.data(), &FrameConvertor::finished, [=]() { _convertor.clear(); });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(_convertor.data(), &FrameConvertor::newFrame, this, &ViewerData::newFrame);

    thread->start();
}

/*!
 * Destructor.
 */
ViewerData::~ViewerData()
{
    _convertor.data()->stop();
}

