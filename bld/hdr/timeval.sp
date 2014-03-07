:segment DOS
#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED

struct timeval {
    long        tv_sec;     /* seconds */
    long        tv_usec;    /* and microseconds */
};
:elsesegment QNX | LINUX
struct timeval {
    time_t      tv_sec;     /* seconds */
    long        tv_usec;    /* and microseconds */
};
:endsegment

/*
 * Operations on timevals.
 *
 * NB: timercmp does not work for >= or <=.
 */
#define timerisset(tvp)         ((tvp)->tv_sec || (tvp)->tv_usec)
#define timercmp(tvp, uvp, cmp) ((tvp)->tv_sec cmp (uvp)->tv_sec) || \
    (tvp)->tv_sec == (uvp)->tv_sec && (tvp)->tv_usec cmp (uvp)->tv_usec )
#define timerclear(tvp)         ((tvp)->tv_sec = (tvp)->tv_usec = 0)
:segment DOS

#endif
:endsegment
