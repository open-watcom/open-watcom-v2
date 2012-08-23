#ifndef __SYS_WTIME_H
#define __SYS_WTIME_H

/*
 * The naming <sys/wtime.h> is required for those compilers that
 * have <sys/time.h> in the usual place but doesn't define
 * the following.
 */

#include <time.h>

#ifdef __BORLANDC__
#undef timezone   /* a macro in bcc 5+ */
#endif

#ifndef __SYS_CDEFS_H
#include <sys/cdefs.h>
#endif

#ifdef __DJGPP__
  #include <sys/time.h>
  #include <sys/times.h>
#else

  struct timeval {
         time_t tv_sec;
         long   tv_usec;
       };

  struct timezone {
         int tz_minuteswest;
         int tz_dsttime;
       };

  struct tms {
         unsigned long tms_utime;
         unsigned long tms_cstime;
         unsigned long tms_cutime;
         unsigned long tms_stime;
       };

  __BEGIN_DECLS

  #define ITIMER_REAL  0
  #define ITIMER_PROF  1

  struct itimerval {
         struct timeval it_interval;  /* timer interval */
         struct timeval it_value;     /* current value */
       };

  extern int getitimer (int, struct itimerval *);
  extern int setitimer (int, struct itimerval *, struct itimerval *);

  extern int gettimeofday (struct timeval *tp, struct timezone *tz);

  __END_DECLS

#endif      /* !__DJGPP__ */

#ifndef HZ
#define HZ 18.2F
#endif

__BEGIN_DECLS

unsigned long net_times (struct tms *buffer);

int gettimeofday2 (struct timeval *tv, struct timezone *tz);

__END_DECLS

/*
 * Operations on timevals.
 *
 * NB: timercmp does not work for >= or <=.
 */
#ifndef timerisset
  #define timerisset(tvp)         ((tvp)->tv_sec || (tvp)->tv_usec)

  #define timercmp(tvp, uvp, cmp) ((tvp)->tv_sec cmp (uvp)->tv_sec || \
                                   ((tvp)->tv_sec == (uvp)->tv_sec &&  \
                                    (tvp)->tv_usec cmp (uvp)->tv_usec))

  #define timerclear(tvp)         (tvp)->tv_sec = (tvp)->tv_usec = 0
#endif

#endif  /* !__SYS_WTIME_H */
