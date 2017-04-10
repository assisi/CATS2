#ifndef CATS2_TIMER_HPP
#define CATS2_TIMER_HPP

#include <QtCore/QDebug>

#include <chrono>

/*!
 * A class that resets itself for a time moment and then upon request compares
 * the current time with the stored and the provided timeout.
 */
class Timer
{
public:
    //! Constructor.
    explicit Timer():
        m_startTime(std::chrono::milliseconds::zero())
    { }

    //! Checks if the timer is set.
    bool isSet() const { return (m_startTime.count() > 0); }

    //! Resets the timer.
    void reset()
    {
        m_startTime = std::chrono::duration_cast<std::chrono::milliseconds>
                (std::chrono::system_clock::now().time_since_epoch());
    }

    //! Clears the time, it's need to be reset to be used.
    void clear() { m_startTime = std::chrono::milliseconds::zero(); }

    //! Checks for the timeout.
    bool isTimedOutSec(double timeOutSec)
    {
        if (isSet()) {
            return (runTimeSec() >  timeOutSec);
        } else {
            qDebug() << "Timer is not set";
            return true;
        }
    }

    //! The timer runtime in seconds.
    double runTimeSec() const
    {
        if (isSet()) {
            std::chrono::milliseconds timeNow = std::chrono::duration_cast
                    <std::chrono::milliseconds>(std::chrono::system_clock::now()
                                                .time_since_epoch());
            return std::chrono::duration_cast
                    <std::chrono::duration<double,std::ratio<1,1>>>
                    (timeNow - m_startTime).count();
       } else {
            qDebug() << "Timer is not set";
            return 0.;
        }
    }

private:
    //! Defines the moment when the program started as a time since epoch.
    std::chrono::milliseconds m_startTime;
};

#endif // CATS2_TIMER_HPP

