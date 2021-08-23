/*
 *  gettimeofday() for non-djgpp targets
 *  gettimeofday2() for higher accuracy timing
 *
 *  G. Vanem <giva@bgnett.no>
 *
 *  10.aug 1996 - Created
 *
 */

#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <time.h>
#include <dos.h>

#include "wattcp.h"
#include "wdpmi.h"
#include "misc.h"
#include "gettod.h"
#include "iregs.h"

/*
 * Timezone defines for Watcom, Borland <= 3.1 and MSC <= 6.0
 */
#if defined(__WATCOMC__)
#define _timezone timezone
#endif

#if defined(__TURBOC__) && (__TURBOC__ <= 0x410)  /* TCC/BCC <= 3.1 */
#define _timezone timezone
#endif

#if defined(_MSC_VER) && (_MSC_VER <= 600)
#define _timezone timezone
#endif

static void get_zone (struct timezone *tz, time_t now)
{
  struct tm *tm = localtime (&now);

  if (tm)
  {
#ifdef __DJGPP__
    tz->tz_minuteswest = -60 * tm->__tm_gmtoff;
#else
    tz->tz_minuteswest = -60 * (int)_timezone;
#endif
    tz->tz_dsttime = tm->tm_isdst;
  }
}

#if (DOSX == 0)
int gettimeofday (struct timeval *tv, struct timezone *tz)
{
  IREGS  regs;
  struct tm   tm;

  if (!tv)
     return (-1);

  memset (&regs, 0, sizeof(regs));
  hiREG(regs.r_ax, 0x2C);
  GEN_RM_INTERRUPT (0x21, &regs);

  tv->tv_usec = loBYTE (regs.r_dx) * 10000L;
  tm.tm_sec   = hiBYTE (regs.r_dx);
  tm.tm_min   = loBYTE (regs.r_cx);
  tm.tm_hour  = hiBYTE (regs.r_cx);

  hiREG(regs.r_ax, 0x2A);
  GEN_RM_INTERRUPT (0x21, &regs);

  tm.tm_mday  = loBYTE (regs.r_dx);
  tm.tm_mon   = hiBYTE (regs.r_dx) - 1;
  tm.tm_year  = (regs.r_cx & 0x7FF) - 1900;
  tm.tm_wday  = tm.tm_yday = 0;
  tm.tm_isdst = -1;

  tv->tv_sec = mktime (&tm);

  if (tz)
     get_zone (tz, tv->tv_sec);
  return (0);
}

#elif (DOSX & PHARLAP)
int gettimeofday (struct timeval *tv, struct timezone *tz)
{
  IREGS  regs;
  struct tm tm;

  if (!tv)
     return (-1);

  memset (&regs, 0, sizeof(regs));
  regs.r_ax = 0x2C00;
  GEN_RM_INTERRUPT (0x21, &regs);

  tv->tv_usec = loBYTE (regs.r_dx) * 10000L;
  tm.tm_sec   = hiBYTE (regs.r_dx);
  tm.tm_min   = loBYTE (regs.r_cx);
  tm.tm_hour  = hiBYTE (regs.r_cx);

  regs.r_ax = 0x2A00;
  GEN_RM_INTERRUPT (0x21, &regs);

  tm.tm_mday  = loBYTE (regs.r_dx);
  tm.tm_mon   = hiBYTE (regs.r_dx) - 1;
  tm.tm_year  = (regs.r_cx & 0x7FF) - 1900;
  tm.tm_wday  = tm.tm_yday = 0;
  tm.tm_isdst = -1;

  tv->tv_sec = mktime (&tm);
  if (tz)
     get_zone (tz, tv->tv_sec);
  return (0);
}

#elif (DOSX & (DOS4GW|WDOSX)) && !defined(__GNUC__)
int gettimeofday (struct timeval *tv, struct timezone *tz)
{
  IREGS  regs;
  struct tm   tm;

  if (!tv)
     return (-1);

  memset (&regs, 0, sizeof(regs));
  regs.r_ax = 0x2C00;
  GEN_RM_INTERRUPT (0x21, &regs);

  tv->tv_usec = loBYTE (regs.r_dx) * 10000;
  tm.tm_sec   = hiBYTE (regs.r_dx);
  tm.tm_min   = loBYTE (regs.r_cx);
  tm.tm_hour  = hiBYTE (regs.r_cx);

  regs.r_ax = 0x2A00;
  GEN_RM_INTERRUPT (0x21, &regs);

  tm.tm_mday  = loBYTE (regs.r_dx);
  tm.tm_mon   = hiBYTE (regs.r_dx) - 1;
  tm.tm_year  = (regs.r_cx & 0x7FF) - 1900;
  tm.tm_wday  = tm.tm_yday = 0;
  tm.tm_isdst = -1;

  tv->tv_sec = mktime (&tm);
  if (tz)
     get_zone (tz, tv->tv_sec);
  return (0);
}

#elif (DOSX & POWERPAK)
int gettimeofday (struct timeval *tv, struct timezone *tz)
{
  UNFINISHED();
}

#endif   /* DOSX == 0 */

/*
 * A high-resolution [1us] version of gettimeofday() needed in
 * select_s() etc.
 */
int gettimeofday2 (struct timeval *tv, struct timezone *tz)
{
#if defined(HAVE_UINT64)
  if (has_8254)
  {
    static time_t secs = 0;  /* seconds since epoch until last midnight */
    static uint64 last = 0;
    uint64 usecs = microsec_clock();   /* usec day-clock */

    if (secs == 0 || usecs < last)     /* not init or wrapped */
    {
      secs  = time (NULL);
      secs -= (secs % (24*3600));
    }
    last = usecs;
    tv->tv_sec  = (usecs / (uint64)1000000) + (uint64)secs;
    tv->tv_usec = (usecs % (uint64)1000000);
    if (tz)
       get_zone (tz, tv->tv_sec);
    return (0);
  }
#endif
  return gettimeofday (tv, tz);
}

#if defined(TEST_PROG)

#include <unistd.h>

int main (void)
{
  DWORD          loops = 0;
  struct timeval delta = { 0, 0 };

  init_misc();

  while (!kbhit())
  {
    struct timeval tv;
    struct timeval last;

    gettimeofday2 (&tv, NULL);

    if (loops > 0)
         delta = timeval_diff(&tv, &last);
    last.tv_sec  = tv.tv_sec;
    last.tv_usec = tv.tv_usec;

    printf ("%10u.%06lu, %u.%06lu, (%.19s)\n",
            tv.tv_sec, tv.tv_usec, delta.tv_sec, delta.tv_usec, ctime(&tv.tv_sec));
    usleep (100000);
    loops++;
  }
  return (0);
}
#endif
