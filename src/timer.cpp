#include <sys/time.h>
#include <time.h>
#include <ctime>
#include <cstdio>
#include <cmath>
#include "timer.h"

inline double toDouble(const struct timeval & tm)
{
    return (double) tm.tv_sec + (double) tm.tv_usec * 1.0e-6;
}

double timeNowAsDouble()
{
    struct timeval tm = {};
    gettimeofday(&tm, NULL);
    return toDouble(tm);
}

// Format HH:MM::SS.sss
std::string hoursMinutesSeconds(double t)
{
    double hours = std::floor(t / 3600.0);
    t = t - hours * 3600.0;
    double minutes = std::floor(t / 60.0);
    t = t - minutes * 60.0;
    
    const int bufsize = 16;
    char buffer[bufsize];

    snprintf(buffer, bufsize, "%02u:%02u:%06.3f",
             (unsigned int) hours, (unsigned int) minutes, t);

    return std::string(buffer);
}

