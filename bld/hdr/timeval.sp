#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED

:segment LINUX
:include time_t.sp

:endsegment
:include susecond.sp

struct timeval {
:segment LINUX
    time_t      tv_sec;     /* seconds */
:elsesegment 
    long        tv_sec;     /* seconds */
:endsegment
    suseconds_t tv_usec;    /* and microseconds */
};

/*
 * Operations on timevals.
 *
 * NB: timercmp does not work for >= or <=.
 */
#define timerisset(tvp)         ((tvp)->tv_sec || (tvp)->tv_usec)
#define timercmp(tvp, uvp, cmp) ((tvp)->tv_sec cmp (uvp)->tv_sec) || \
    (tvp)->tv_sec == (uvp)->tv_sec && (tvp)->tv_usec cmp (uvp)->tv_usec )
#define timerclear(tvp)         ((tvp)->tv_sec = (tvp)->tv_usec = 0)

#endif /* !_TIMEVAL_DEFINED */
