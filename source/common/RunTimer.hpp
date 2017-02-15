#ifndef CATS2_RUN_TIMER_HPP
#define CATS2_RUN_TIMER_HPP

#include <chrono>

/*!
 * Provides the time from the program start.
 */
// TODO : integrate Timer class here as a member managing the timing
class RunTimer
{
public:
    //! The singleton getter.
    static RunTimer& get();

    //! Saves the current time as the program start time.
    void init();

    // delete copy and move constructors and assign operators
    //! Copy constructor.
    RunTimer(RunTimer const&) = delete;
    //! Move constructor.
    RunTimer(RunTimer&&) = delete;
    //! Copy assignment.
    RunTimer& operator=(RunTimer const&) = delete;
    //! Move assignment.
    RunTimer& operator=(RunTimer &&) = delete;

public:
    //! The runtime in seconds to the provided timestamp.
    double runtimeSecTo(std::chrono::milliseconds timestamp);
    //! The runtime in seconds to the current time moment.
    double currentRuntimeSec();

private:
    //! Constructor. Defining it here prevents construction.
    RunTimer();
    //! Destructor. Defining it here prevents unwanted destruction.
    ~RunTimer();

private:
    //! Defines the moment when the program started as a time since epoch.
    std::chrono::milliseconds m_startTime;
    //! Initialization status.
    bool m_initialized;
};

#endif // CATS2_RUN_TIMER_HPP
