#ifndef __TIMER__
#define __TIMER__

#include <chrono>
#include <ctime>

class ProcessorTimer {
public:
    ProcessorTimer() = default;
    ~ProcessorTimer() = default;
    
    static inline ProcessorTimer makeRunningTimer();

    inline void start();
    inline void stop();
    inline double elapsed();
    
protected:
    clock_t start_time;
    clock_t end_time;
    bool running = false;
    bool valid   = false;
};

class WallClockTimer {
public:
    WallClockTimer() = default;
    ~WallClockTimer() = default;

    static inline WallClockTimer makeRunningTimer();

    void start();
    void stop();
    double elapsed();

protected:
    using wallclock = std::chrono::system_clock;
    using timepoint = std::chrono::time_point<wallclock>;

    timepoint startTimePoint;
    timepoint endTimePoint;

    bool running = false;
    bool valid   = false;
};

// Inline implementations

inline ProcessorTimer ProcessorTimer::makeRunningTimer()
{
    ProcessorTimer timer;
    timer.start();
    return timer;
}

inline void ProcessorTimer::start()
{
    start_time = std::clock();
    running = true;
    valid = true;
}

inline void ProcessorTimer::stop()
{
    end_time = std::clock();
    running = false;
}

inline double ProcessorTimer::elapsed()
{
    if(!valid) {
        return 0.0;
    }
    
    if(running) {
        end_time = std::clock();
    }
    
    return (double) (end_time - start_time) / CLOCKS_PER_SEC;
}

inline WallClockTimer WallClockTimer::makeRunningTimer()
{
    WallClockTimer timer;
    timer.start();
    return timer;
}

inline void WallClockTimer::start()
{
    startTimePoint = wallclock::now();
    running = true;
    valid = true;
}

inline void WallClockTimer::stop()
{
    endTimePoint = wallclock::now();
    running = false;
}

inline double WallClockTimer::elapsed()
{
    if(!valid) {
        return 0.0;
    }
    
    if(running) {
        endTimePoint = wallclock::now();
    }

    std::chrono::duration<double> elapsedTime = endTimePoint - startTimePoint;

    return elapsedTime.count();
}


#endif
