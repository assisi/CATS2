#include "RunTimer.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
RunTimer::RunTimer() :
    m_startTime(std::chrono::milliseconds::zero()),
    m_initialized(false)
{
}

/*!
 * Destructor.
 */
RunTimer::~RunTimer()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
}

/*!
 * The singleton getter.
 */
RunTimer& RunTimer::get()
{
    static RunTimer instance;
    return instance;
}

/*!
 * Saves the current time as the program start time.
 */
void RunTimer::init()
{
    m_startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    m_initialized = true;
}

/*!
 * The runtime in seconds to the current time moment.
 */
double RunTimer::currentRuntimeSec()
{
    return runtimeSecTo(std::chrono::duration_cast<std::chrono::milliseconds>
                        (std::chrono::system_clock::now().time_since_epoch()));
}

/*!
 * The runtime in seconds to the provided timestamp.
 */
double RunTimer::runtimeSecTo(std::chrono::milliseconds timestamp)
{
    if (m_initialized)
        return std::chrono::duration_cast<std::chrono::duration<double,std::ratio<1,1>>>(timestamp - m_startTime).count();
    else {
        qDebug() << Q_FUNC_INFO << "The timer is not initialized.";
        return 0;
    }
}
