#ifndef _TIMESPEC_DEFINED
#define _TIMESPEC_DEFINED

:include time_t.sp

struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

#endif
