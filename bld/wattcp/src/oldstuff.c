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
#include "oldstuff.h"

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
void sock_sturdy (sock_type *sk, int level)
{
  sk->tcp.rigid = level;
  if (sk->tcp.rigid < sk->tcp.stress)
     sock_abort (sk);
}

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

