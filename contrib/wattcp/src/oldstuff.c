/*
 * Some old functions are collected here
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dos.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "pc_cbrk.h"
#include "pctcp.h"
#include "misc.h"

/*
 * Actually simply a macro in <tcp.h>
 */
int tcp_cbrk (int mode)
{
  return tcp_cbreak (mode);
}

/*
 * Not needed in Watt-32. Return timeout from ticks.
 */
unsigned long set_ttimeout (unsigned ticks)
{
  int   tmp = has_8254;
  DWORD now;

  has_8254 = 0;
  now = set_timeout (0);
  has_8254 = tmp;
  return (ticks + now);
}

/*
 * Keep the socket open inspite of receiving "ICMP Unreachable"
 */
void sock_sturdy (sock_type *s, int level)
{
  s->tcp.rigid = level;
  if (s->tcp.rigid < s->tcp.stress)
     sock_abort (s);
}

/*
 * Some time/date conversion function only(?) found in Borland libs.
 * Only(?) djgpp besides Borland have "struct time" etc.
 */
#if defined(__DJGPP__)
time_t dostounix (struct date *d, struct time *t)
{
  struct tm tm;

  if (!d || !t)
     return (0);

  tm.tm_year  = d->da_year - 1900;
  tm.tm_mon   = d->da_mon - 1;
  tm.tm_mday  = d->da_day;
  tm.tm_isdst = 0;
  tm.tm_hour  = t->ti_hour;
  tm.tm_min   = t->ti_min;
  tm.tm_sec   = t->ti_sec;
  return mktime (&tm);
}

void unixtodos (time_t time, struct date *d, struct time *t)
{
  struct tm *tm = localtime (&time);

  if (d && tm)
  {
    d->da_year = tm->tm_year;
    d->da_mon  = tm->tm_mon;
    d->da_day  = tm->tm_mday;
  }
  if (t && tm)
  {
    t->ti_hour = tm->tm_hour;
    t->ti_min  = tm->tm_min;
    t->ti_sec  = tm->tm_sec;
    t->ti_hund = 0;
  }
}
#endif


#if defined(TEST_PROG)

int main (void)
{
  struct date d;
  struct time t;
  time_t now = time (NULL);

  getdate (&d);
  gettime (&t);
  printf ("time now is: %s", ctime(&now));
  now = dostounix (&d, &t);
  printf ("dostounix(): %s", ctime(&now));
  printf ("unixtodos(): %02d/%02d/%04d %02d:%02d:%02d\n",
          d.da_day,  d.da_mon, d.da_year,
          t.ti_hour, t.ti_min, t.ti_sec);
  return (0);
}
#endif

