/*
 *  Module for handling timers.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dos.h>
#include <sys/wtime.h>

#include "wattcp.h"
#include "ioport.h"
#include "strings.h"
#include "gettod.h"
#include "misc.h"
#include "timer.h"

int has_8254  = 0; /* Do we have a working 8254 timer chip? */
int has_rdtsc = 0; /* Do we have a CPU with RDTSC instruction? */

/*
 * System clock at BIOS location 40:6C (dword). Counts upwards.
 */
#define BIOS_CLK 0x46C

static DWORD date    = 0;
static DWORD date_ms = 0;

/*
 * init_timers() - setup timer handling.
 * Called from init_misc()
 */
void init_timers (void)
{
#if (DOSX)         /* problems using 64-bit types in small/large models */
  has_8254 = 1;    /* !! to-do: check if 8254 PIT is really working */
#endif

  chk_timeout (0UL); /* init 'date' variables */
}

/*
 * When using Watt-32 together with programs that reprogram the PIT
 * (8254 timer chip), the millisec_clock() function may return wrong
 * values. In such cases it's best to use old-style 55ms timers.
 * Using Watt-32 with Allegro graphics library is one such case where
 * application program should call `hires_timer(0)'.
 *
 * !!to-do: A viable solution is to use the RDTSC instruction
 *          on Pentiums when that is detected (has_rdtsc=1).
 *          64-bit timestamps will wrap after ~830 years on a
 *          700MHz PC !!
 */
void hires_timer (int on)
{
  has_8254 = on;
  chk_timeout (0UL);
}

/*
 * Compare two timers with expirary at 't1' and 't2'.
 *    returns -1 if t1 expires before t2
 *             0 if t1 == t2
 *            +1 if t1 expires after t2
 *
 * FIX-ME: This logic fails when timers approaches ULONG_MAX
 *         after approx. 50 days.
 */
int cmp_timers (DWORD t1, DWORD t2)
{
  if (t1 == t2)
     return (0);
  return (t1 < t2 ? -1 : +1);
}

/*
 * The following 2 routines are modified versions from KA9Q NOS
 * by Phil Karn.
 *
 * clockbits() - Read low order bits of timer 0 (the TOD clock)
 * This works only for the 8254 chips used in ATs and 386s.
 *
 * The timer 0 runs in mode 3 (square wave mode), counting down
 * by twos, twice for each cycle. So it is necessary to read back the
 * OUTPUT pin to see which half of the cycle we're in. I.e., the OUTPUT
 * pin forms the most significant bit of the count. Unfortunately,
 * the 8253 in the PC/XT lacks a command to read the OUTPUT pin...
 */
static DWORD clockbits (void)
{
  VOLATILE DWORD stat, count;

  do
  {
    _outportb (0x43, 0xC2);        /* Read Back Command (timer 0, 8254 only) */
    stat   = _inportb (0x40);      /* get status of timer 0 */
    count  = _inportb (0x40);      /* LSB of count */
    count |= _inportb (0x40) << 8; /* MSB of count */
  }
  while (stat & 0x40);             /* reread if NULL COUNT bit set */

  stat = (stat & 0x80) << 8;       /* Shift OUTPUT to msb of 16-bit word */
  count >>= 1;                     /* count /= 2 */
  if (count == 0)
     return (stat ^ 0x8000UL);     /* return complement of OUTPUT bit */
  return (count | stat);           /* Combine OUTPUT with counter */
}

/*
 * Return hardware time-of-day in milliseconds. Resolution is improved
 * beyond 55 ms (the clock tick interval) by reading back the instantaneous
 * 8254 counter value and combining it with the clock tick counter.
 *
 * Reading the 8254 is a bit tricky since a tick could occur asynchronously
 * between the two reads in clockbits(). The tick counter is examined before
 * and after the hardware counter is read. If the tick counter changes, try
 * again.
 * Note: the hardware counter (lo) counts down from 65536 at a rate of
 *       1.19318 MHz (4.77/4). System tick count (hi) counts up.
 *       Fraction 11/13125 gives us a milli-sec rate count.
 */
DWORD millisec_clock (void)
{
  DWORD hi, lo;
  do
  {
    hi = peekl (0, BIOS_CLK);
    lo = clockbits();
  }
  while (hi != peekl(0, BIOS_CLK));   /* tick count changed, try again */

#ifdef HAVE_UINT64
  {
    uint64 x = ((uint64)hi << 16) - lo;
    return (x * 11UL / 13125UL);
  }
#else   /* Emulating this would be slow. We'd better have a math-processor */
  {
    double x = ldexp ((double)hi, 16) - (double)lo;  /* x = hi*2^16 - lo */
    return (DWORD) (x * 4.0 / 4770.0);
  }
#endif
}

#if defined(HAVE_UINT64)
/*
 * Return hardware time-of-day in microseconds.
 */
uint64 microsec_clock (void)
{
  DWORD  hi, lo;
  uint64 x;
  do
  {
    hi = peekl (0, BIOS_CLK);
    lo = clockbits();
  }
  while (hi != peekl(0, BIOS_CLK));   /* tick count changed, try again */

  x = ((uint64)hi << 16) - lo;
  return (x * 4000000 / 4770000);
}
#endif


/*
 * Return time for when given timeout (msec) expires
 */
DWORD set_timeout (DWORD msec)
{
#if defined(USE_NEW_TIMERS) && defined(HAVE_UINT64)
  struct timeval now;
  DWORD  usec = microsec_clock() % 1000;

  gettimeofday (&now, NULL);   /* 55msec resolution */
  return (msec + 1000 * now.tv_sec + (now.tv_usec + usec) / 1000);

#else
  if (!has_8254)
  {
    DWORD ticks;

    if (msec > 0 && msec < 55)
         ticks = 1;
    else ticks = msec / 55UL;
    return (ticks + date + (DWORD)peekl (0,BIOS_CLK));
  }
  return (msec + date_ms + millisec_clock());
#endif
}

/*
 * Check if milli-sec value has expired:
 *   returns 1 timer expired
 *           0 not expired (or value not set)
 */
int chk_timeout (DWORD value)
{
#if defined(USE_NEW_TIMERS) && defined(HAVE_UINT64)
  if (value == 0UL)
     return (0);
  return (set_timeout(0) >= value);
#else
  static char  oldHour = -1;
  static DWORD now_ms;
  static DWORD now;
  char   hour;

  now  = (DWORD) peekl (0, BIOS_CLK);
  hour = (char)(now >> 16U);

  if (hour != oldHour)
  {
    if (hour < oldHour)            /* midnight passed */
    {
      #define CORR 290UL           /* experimental correction */
      date    += 1572750UL + CORR; /* Update date (0x1800B0UL) */
      date_ms += 3600*24*1000UL;   /* ~2^26 */
    }
    oldHour = hour;
  }

  if (value == 0L)        /* timer not initialised */
     return (0);          /* (or stopped) */

  if (has_8254)
  {
    now_ms = millisec_clock();
    return (now_ms + date_ms >= value);
  }
  now += date;            /* date extend current time */
  return (now >= value);  /* return true if expired */
#endif
}

/*
 * Must be called by user right before or after a time change occurs.
 */
int set_timediff (long msec)
{
  date_ms -= msec;
  date    -= msec/55;
  return (0);
}

/*
 * Return time difference between timers 'now' and 't'.
 * Check for day rollover. Max timer distance is 24 hours.
 * This function should be called immediately after chk_timeout()
 * is called.
 */
DWORD get_timediff (DWORD now, DWORD tim)
{
#ifdef USE_NEW_TIMERS
  return (now - tim);
#else
  DWORD dt;

  if (now < tim)           /* passed midnight */
       dt = tim - now;
  else dt = now - tim;

  if (has_8254)
       dt = dt % (3600*24*1000UL);
  else dt = dt % (1572750UL + CORR);
  return (dt);
#endif
}

/*
 * Return difference (in micro-sec) between timevals `*a' and `*b'
 */
struct timeval timeval_diff (const struct timeval *a, const struct timeval *b)
{
  struct timeval d;

  d.tv_sec  = a->tv_sec - b->tv_sec;
  d.tv_usec = a->tv_usec - b->tv_usec;
  if( a->tv_sec < b->tv_sec ) {
    if( d.tv_usec > 0 ) {
      d.tv_sec++;
      d.tv_usec -= 1000000L;
    }
  } else {
    if( d.tv_usec < 0 ) {
      d.tv_sec--;
      d.tv_usec += 1000000L;
    }
  }
  return (d);
}


#if defined(USE_DEBUG)
/*
 * time_str() - return string "x.xx" for timeout value.
 */
const char *time_str (DWORD val)
{
  static char buf[30];

  if (has_8254 || has_rdtsc)
       sprintf (buf, "%.3f", (double)val/1000.0);
  else sprintf (buf, "%.2f", (double)val/18.2);
  return (buf);
}
#endif /* USE_DEBUG */

/*
 * The following was contributed by
 * "Alain" <alainm@pobox.com>
 *
 */
#ifdef NOT_USED
#define TICKS_DAY 1573067UL   /* Number of ticks in one day */

DWORD ms_clock (void)
{
  static DWORD lastTick, tickOffset;
  static char firstTime = 1;
  DWORD  tick = peekl (0, 0x46C);

  if (firstTime)
  {
    firstTime = 0;
    lastTick = tickOffset = tick;
    return (0);
  }
  if (lastTick > tick)        /* day changed */
     tickOffset -= TICKS_DAY;
  lastTick = tick;
  return (tick - tickOffset);
}
#endif
