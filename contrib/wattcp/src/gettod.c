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
#include "misc.h"
#include "gettod.h"

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
  union  REGS reg;
  struct tm   tm;

  if (!tv)
     return (-1);

  reg.h.ah = 0x2C;
  int86 (0x21, &reg, &reg);

  tv->tv_usec = reg.h.dl * 10000L;
  tm.tm_sec   = reg.h.dh;
  tm.tm_min   = reg.h.cl;
  tm.tm_hour  = reg.h.ch;

  reg.h.ah = 0x2A;
  int86 (0x21, &reg, &reg);

  tm.tm_mday  = reg.h.dl;
  tm.tm_mon   = reg.h.dh - 1;
  tm.tm_year  = (reg.x.cx & 0x7FF) - 1900;
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
  SWI_REGS  reg;
  struct tm tm;

  if (!tv)
     return (-1);

  reg.eax = 0x2C00;
  _dx_real_int (0x21, &reg);

  tv->tv_usec = loBYTE (reg.edx) * 10000L;
  tm.tm_sec   = hiBYTE (reg.edx);
  tm.tm_min   = loBYTE (reg.ecx);
  tm.tm_hour  = hiBYTE (reg.ecx);

  reg.eax = 0x2A00;
  _dx_real_int (0x21, &reg);

  tm.tm_mday  = loBYTE (reg.edx);
  tm.tm_mon   = hiBYTE (reg.edx) - 1;
  tm.tm_year  = (reg.ecx & 0x7FF) - 1900;
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
  union  REGS reg;
  struct tm   tm;

  if (!tv)
     return (-1);

  reg.x.eax = 0x2C00;
  int386 (0x21, &reg, &reg);

  tv->tv_usec = reg.h.dl * 10000;
  tm.tm_sec   = reg.h.dh;
  tm.tm_min   = reg.h.cl;
  tm.tm_hour  = reg.h.ch;

  reg.x.eax = 0x2A00;
  int386 (0x21, &reg, &reg);

  tm.tm_mday  = reg.h.dl;
  tm.tm_mon   = reg.h.dh - 1;
  tm.tm_year  = (reg.x.ecx & 0x7FF) - 1900;
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
