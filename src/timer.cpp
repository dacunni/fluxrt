#include <sys/time.h>
#include <time.h>
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

