#ifndef __GETTIMEOFDAY_H
#define __GETTIMEOFDAY_H

#if !defined(__DJGPP__) && !defined(__SYS_WTIME_H)
struct timeval {
       time_t tv_sec;
       long   tv_usec;
     };

struct timezone {
       int tz_minuteswest;
       int tz_dsttime;
     };

extern int gettimeofday (struct timeval *tv, struct timezone *tz);
#endif

extern int gettimeofday2 (struct timeval *tv, struct timezone *tz);

#endif
